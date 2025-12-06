#ifndef __MONOCULAR_SLAM_NODE_HPP__
#define __MONOCULAR_SLAM_NODE_HPP__

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

#include <cv_bridge/cv_bridge.h>

#include "Frame.h"
#include "Map.h"
#include "System.h"
#include "Tracking.h"

#include "utility.hpp"

#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/transform_broadcaster.h"

class MonocularSlamNode : public rclcpp::Node {
public:
  MonocularSlamNode(ORB_SLAM3::System *pSLAM);

  ~MonocularSlamNode();

private:
  using ImageMsg = sensor_msgs::msg::Image;

  void GrabImage(const sensor_msgs::msg::Image::SharedPtr msg);

  ORB_SLAM3::System *m_SLAM;

  cv_bridge::CvImagePtr m_cvImPtr;

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr m_image_subscriber;

  // Publishers
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr
      m_pose_publisher;
  std::unique_ptr<tf2_ros::TransformBroadcaster> m_tf_broadcaster;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr
      m_map_points_publisher;
  rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr m_map_publisher;
};

#endif
