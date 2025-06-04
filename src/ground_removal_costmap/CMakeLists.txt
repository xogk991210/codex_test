cmake_minimum_required(VERSION 3.5)
project(ground_removal_costmap)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(tf2_ros REQUIRED)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
)

add_executable(ground_removal_node src/ground_removal_node.cpp)
ament_target_dependencies(ground_removal_node
  rclcpp sensor_msgs nav_msgs pcl_conversions pcl_ros tf2_ros
)

target_link_libraries(ground_removal_node ${PCL_LIBRARIES})

install(TARGETS ground_removal_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
