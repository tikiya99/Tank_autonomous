#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "monocular-slam-node.hpp"
#include "rclcpp/rclcpp.hpp"

#include "System.h"

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr
        << "\nUsage: ros2 run orbslam mono path_to_vocabulary path_to_settings"
        << std::endl;
    return 1;
  }

  rclcpp::init(argc, argv);

  // Enable visualization to show the native ORB-SLAM3 viewer
  bool visualization = true;
  ORB_SLAM3::System SLAM(argv[1], argv[2], ORB_SLAM3::System::MONOCULAR,
                         visualization);

  auto node = std::make_shared<MonocularSlamNode>(&SLAM);
  std::cout << "============================ " << std::endl;

  rclcpp::spin(node);
  rclcpp::shutdown();

  return 0;
}
