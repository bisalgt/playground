#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>

int
main (int argc, char** argv)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);

  if (pcl::io::loadPCDFile<pcl::PointXYZ> ("../../demeaned_cloud.pcd", *cloud) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file kitti.pcd \n");
    return (-1);
  }
  std::cout << "Loaded "
            << cloud->width * cloud->height
            << " data points from kitti.pcd"
            << std::endl;
  

    Eigen::Matrix3f covarianceMatrix;
    pcl::computeCovarianceMatrix(*cloud, covarianceMatrix);

    std::cout << "Covariance Matrix:\n" << covarianceMatrix << std::endl;
    std::cout << "Covariance Matrix size: " << covarianceMatrix.rows() << "x" << covarianceMatrix.cols() << std::endl;
    return 0;
}
