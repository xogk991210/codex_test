#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "pcl_conversions/pcl_conversions.h"
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/voxel_grid.h>

class GroundRemovalNode : public rclcpp::Node
{
public:
  GroundRemovalNode() : Node("ground_removal_node")
  {
    cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
      "ouster/points", rclcpp::SensorDataQoS(),
      std::bind(&GroundRemovalNode::cloudCallback, this, std::placeholders::_1));

    costmap_pub_ = create_publisher<nav_msgs::msg::OccupancyGrid>(
      "ground_costmap", rclcpp::SystemDefaultsQoS());

    declare_parameter<double>("voxel_leaf", 0.1);
    declare_parameter<double>("plane_distance", 0.2);
    declare_parameter<int>("grid_size", 100);
    declare_parameter<double>("grid_resolution", 0.1);
  }

private:
  void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
  {
    // Convert to PCL point cloud
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::fromROSMsg(*msg, *cloud);

    // Downsample
    double leaf = get_parameter("voxel_leaf").as_double();
    pcl::VoxelGrid<pcl::PointXYZ> vg;
    vg.setInputCloud(cloud);
    vg.setLeafSize(leaf, leaf, leaf);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
    vg.filter(*cloud_filtered);

    // Ground removal using plane segmentation
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    seg.setOptimizeCoefficients(true);
    seg.setModelType(pcl::SACMODEL_PLANE);
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setDistanceThreshold(get_parameter("plane_distance").as_double());
    pcl::PointIndices::Ptr ground_indices(new pcl::PointIndices);
    pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
    seg.setInputCloud(cloud_filtered);
    seg.segment(*ground_indices, *coefficients);

    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud(cloud_filtered);
    extract.setIndices(ground_indices);
    extract.setNegative(true); // remove ground
    pcl::PointCloud<pcl::PointXYZ>::Ptr no_ground(new pcl::PointCloud<pcl::PointXYZ>);
    extract.filter(*no_ground);

    // Create 2D occupancy grid
    int grid_size = get_parameter("grid_size").as_int();
    double resolution = get_parameter("grid_resolution").as_double();
    std::vector<int8_t> grid(grid_size * grid_size, 0);

    for (const auto &p : no_ground->points) {
      int x = static_cast<int>((p.x / resolution) + grid_size / 2);
      int y = static_cast<int>((p.y / resolution) + grid_size / 2);
      if (x >= 0 && x < grid_size && y >= 0 && y < grid_size) {
        grid[y * grid_size + x] = 100;
      }
    }

    nav_msgs::msg::OccupancyGrid costmap;
    costmap.header = msg->header;
    costmap.info.resolution = resolution;
    costmap.info.width = grid_size;
    costmap.info.height = grid_size;
    costmap.info.origin.position.x = -grid_size * resolution / 2.0;
    costmap.info.origin.position.y = -grid_size * resolution / 2.0;
    costmap.info.origin.position.z = 0.0;
    costmap.data = grid;

    costmap_pub_->publish(costmap);
  }

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
  rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr costmap_pub_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<GroundRemovalNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
