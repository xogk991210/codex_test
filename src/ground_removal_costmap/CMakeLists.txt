cmake_minimum_required(VERSION 3.8)
project(ground_removal_costmap)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL 1.7.2 REQUIRED)
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

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
