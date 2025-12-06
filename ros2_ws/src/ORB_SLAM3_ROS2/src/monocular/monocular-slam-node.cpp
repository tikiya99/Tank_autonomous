#include "monocular-slam-node.hpp"

#include <opencv2/core/core.hpp>
#include <sensor_msgs/image_encodings.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

using std::placeholders::_1;

MonocularSlamNode::MonocularSlamNode(ORB_SLAM3::System *pSLAM)
    : Node("ORB_SLAM3_ROS2") {
  m_SLAM = pSLAM;
  // std::cout << "slam changed" << std::endl;
  m_image_subscriber = this->create_subscription<ImageMsg>(
      "camera", 10,
      std::bind(&MonocularSlamNode::GrabImage, this, std::placeholders::_1));

  // Initialize Publishers
  m_pose_publisher =
      this->create_publisher<geometry_msgs::msg::PoseStamped>("pose", 10);
  m_tf_broadcaster = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
  m_map_points_publisher =
      this->create_publisher<sensor_msgs::msg::PointCloud2>("map_points", 10);
  m_map_publisher =
      this->create_publisher<nav_msgs::msg::OccupancyGrid>("map", 10);

  std::cout << "slam changed" << std::endl;
}

MonocularSlamNode::~MonocularSlamNode() {
  // Stop all threads
  m_SLAM->Shutdown();

  // Save camera trajectory
  m_SLAM->SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");
}

void MonocularSlamNode::GrabImage(const ImageMsg::SharedPtr msg) {
  // Copy the ros image message to cv::Mat.
  // ORB-SLAM3 works best with grayscale images
  cv_bridge::CvImagePtr cv_ptr;
  try {
    // Convert to grayscale (mono8) for ORB-SLAM3
    // If the input is already mono8, this won't do any conversion
    // If it's color (bgr8, rgb8, etc.), it will convert to grayscale
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8);
  } catch (cv_bridge::Exception &e) {
    RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
    return;
  }

  // Validate image
  if (cv_ptr->image.empty()) {
    RCLCPP_ERROR(this->get_logger(), "Received empty image!");
    return;
  }

  // Pass the image to ORB-SLAM3
  Sophus::SE3f Tcw = m_SLAM->TrackMonocular(
      cv_ptr->image, Utility::StampToSec(msg->header.stamp));

  // Check if tracking was successful (state 2 is OK)
  if (m_SLAM->GetTrackingState() == 2) {
    // Convert Tcw to Twc (World to Camera)
    Sophus::SE3f Twc = Tcw.inverse();
    Eigen::Vector3f twc = Twc.translation();
    Eigen::Quaternionf qwc = Twc.unit_quaternion();

    // Publish Pose and TF every frame
    geometry_msgs::msg::PoseStamped pose_msg;
    pose_msg.header.stamp = msg->header.stamp;
    pose_msg.header.frame_id = "map";
    pose_msg.pose.position.x = twc.x();
    pose_msg.pose.position.y = twc.y();
    pose_msg.pose.position.z = twc.z();
    pose_msg.pose.orientation.x = qwc.x();
    pose_msg.pose.orientation.y = qwc.y();
    pose_msg.pose.orientation.z = qwc.z();
    pose_msg.pose.orientation.w = qwc.w();
    m_pose_publisher->publish(pose_msg);

    geometry_msgs::msg::TransformStamped tf_msg;
    tf_msg.header.stamp = msg->header.stamp;
    tf_msg.header.frame_id = "map";
    tf_msg.child_frame_id = msg->header.frame_id;
    tf_msg.transform.translation.x = twc.x();
    tf_msg.transform.translation.y = twc.y();
    tf_msg.transform.translation.z = twc.z();
    tf_msg.transform.rotation.x = qwc.x();
    tf_msg.transform.rotation.y = qwc.y();
    tf_msg.transform.rotation.z = qwc.z();
    tf_msg.transform.rotation.w = qwc.w();
    m_tf_broadcaster->sendTransform(tf_msg);

    // Publish Map Points and Occupancy Grid periodically (e.g., every 0.5
    // seconds)
    static auto last_map_pub_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                              last_map_pub_time)
            .count() > 500) {
      last_map_pub_time = now;

      // Get ALL map points
      std::vector<ORB_SLAM3::MapPoint *> map_points = m_SLAM->GetAllMapPoints();

      if (!map_points.empty()) {
        // Publish PointCloud2
        sensor_msgs::msg::PointCloud2 cloud_msg;
        cloud_msg.header.stamp = msg->header.stamp;
        cloud_msg.header.frame_id = "map";
        cloud_msg.height = 1;
        cloud_msg.width = map_points.size();
        cloud_msg.is_dense = false;
        cloud_msg.is_bigendian = false;

        sensor_msgs::PointCloud2Modifier modifier(cloud_msg);
        modifier.setPointCloud2FieldsByString(1, "xyz");
        modifier.resize(map_points.size());

        sensor_msgs::PointCloud2Iterator<float> iter_x(cloud_msg, "x");
        sensor_msgs::PointCloud2Iterator<float> iter_y(cloud_msg, "y");
        sensor_msgs::PointCloud2Iterator<float> iter_z(cloud_msg, "z");

        // Occupancy Grid Parameters
        float resolution = 0.05; // 5 cm
        int width = 400;         // 20 meters
        int height = 400;        // 20 meters
        float origin_x = -10.0;
        float origin_y = -10.0; // origin in map frame

        nav_msgs::msg::OccupancyGrid map_msg;
        map_msg.header.stamp = msg->header.stamp;
        map_msg.header.frame_id = "map";
        map_msg.info.resolution = resolution;
        map_msg.info.width = width;
        map_msg.info.height = height;
        map_msg.info.origin.position.x = origin_x;
        map_msg.info.origin.position.y = origin_y;
        map_msg.info.origin.position.z = 0.0;
        map_msg.info.origin.orientation.w = 1.0;
        map_msg.data.resize(width * height, -1); // Initialize with unknown

        for (auto pMP : map_points) {
          if (pMP && !pMP->isBad()) {
            Eigen::Vector3f pos = pMP->GetWorldPos();
            *iter_x = pos.x();
            *iter_y = pos.y();
            *iter_z = pos.z();
            ++iter_x;
            ++iter_y;
            ++iter_z;

            // Project to 2D Grid (X-Z plane for monocular usually, but let's
            // check) If camera is Z-forward, and map is initialized at
            // identity: World X = Right, World Y = Down, World Z = Forward.
            // Ground plane is X-Z. Height is Y.
            // We want a top-down view of X-Z plane.

            int grid_x = (pos.x() - origin_x) / resolution;
            int grid_y = (pos.z() - origin_y) /
                         resolution; // Note: using origin_y for Z axis offset

            if (grid_x >= 0 && grid_x < width && grid_y >= 0 &&
                grid_y < height) {
              map_msg.data[grid_y * width + grid_x] = 100; // Occupied
            }
          }
        }
        m_map_points_publisher->publish(cloud_msg);
        m_map_publisher->publish(map_msg);
      }
    }
  }
}
