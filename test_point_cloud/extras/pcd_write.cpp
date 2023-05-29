#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

int
  main ()
{
  pcl::PointCloud<pcl::PointXYZ> cloud;

  // Fill in the cloud data
  cloud.width    = 5;
  cloud.height   = 1;
  cloud.is_dense = false;
  cloud.resize (cloud.width * cloud.height);

  for (auto& point: cloud)
  {
    point.x = 1024 * rand () / (RAND_MAX + 1.0f);
    point.y = 1024 * rand () / (RAND_MAX + 1.0f);
    point.z = 1024 * rand () / (RAND_MAX + 1.0f);
  }
  pcl::io::savePCDFileASCII ("test_pcd.pcd", cloud);
  std::cerr << "Saved " << cloud.size () << " data points to test_pcd.pcd." << std::endl;

  for (const auto& point: cloud)
    std::cerr << "    " << point.x << " " << point.y << " " << point.z << std::endl;

//  // Placeholder for the 3x3 covariance matrix at each surface patch
//   Eigen::Matrix3f covariance_matrix;
//   // 16-bytes aligned placeholder for the XYZ centroid of a surface patch
//   Eigen::Vector4f xyz_centroid;

//   // Estimate the XYZ centroid
//   pcl::compute3DCentroid (cloud, xyz_centroid);
//   // Compute the 3x3 covariance matrix
//   pcl::computeCovarianceMatrix (cloud, xyz_centroid, covariance_matrix);

  return (0);
}