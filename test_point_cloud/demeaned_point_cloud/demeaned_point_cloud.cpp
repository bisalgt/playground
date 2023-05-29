#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/common/centroid.h>

int main() {
    // Load the point cloud
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::io::loadPCDFile<pcl::PointXYZ>("../../kitti.pcd", *cloud);

    // Calculate the mean of the point cloud
    Eigen::Vector4f centroid;
    pcl::compute3DCentroid(*cloud, centroid);

    // Demean the point cloud
    pcl::PointCloud<pcl::PointXYZ>::Ptr demeanedCloud(new pcl::PointCloud<pcl::PointXYZ>);
    demeanedCloud->reserve(cloud->size());

    for (std::size_t i = 0; i < cloud->size(); ++i) {
        pcl::PointXYZ point = cloud->points[i];
        point.x -= centroid[0];
        point.y -= centroid[1];
        point.z -= centroid[2];
        demeanedCloud->push_back(point);
    }

    // Save the demeaned point cloud
    pcl::io::savePCDFile<pcl::PointXYZ>("demeaned_cloud.pcd", *demeanedCloud);

    return 0;
}
