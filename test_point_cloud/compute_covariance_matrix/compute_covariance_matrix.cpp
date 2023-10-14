#include <iostream>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/principal_curvatures.h>
#include <pcl/io/pcd_io.h>
#include <pcl/common/centroid.h>

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


    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;

    kdtree.setInputCloud (cloud);

    // Covariance matrices vector
    std::vector<Eigen::Matrix3f> covariance_matrices;

    int k = 50;  // Number of nearest neighbors to consider
    for (int i = 0; i < cloud->size(); ++i) {
        std::vector<int> k_indices(k);
        std::vector<float> k_sqr_distances(k);
        
        // Find the k-nearest neighbors of the current point
        kdtree.nearestKSearch(cloud->points[i], k, k_indices, k_sqr_distances);
        
        // find Centroid for the points for the given indices
        Eigen::Vector4f centroid;
        pcl::compute3DCentroid(*cloud, k_indices, centroid);

        // std::cout<<"Centroid : "<<centroid<<std::endl;

        // Compute the covariance matrix using the neighborhood points
        Eigen::Matrix3f covariance_matrix;
        pcl::computeCovarianceMatrixNormalized(*cloud, k_indices, centroid, covariance_matrix);
        
        // Print the covariance matrix for the current point
        // std::cout << "Covariance matrix for point " << i << ":\n" << covariance_matrix << std::endl;

        covariance_matrices.push_back(covariance_matrix);

}
    std::cout<<"Size of covariance matrices : "<<covariance_matrices.size() << std::endl;
    std::cout<<"Size of matrix 3f : "<< sizeof(Eigen::Matrix3f) << std::endl;

    return 0;
}