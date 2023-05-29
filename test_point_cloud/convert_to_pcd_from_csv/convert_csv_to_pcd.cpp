#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <typeinfo>

int main()
{
    std::string csv_filename = "../../kitti.csv";
    std::string pcd_filename = "kitti.pcd";

    // Read the CSV file
    std::ifstream file(csv_filename);
    std::string line;
    std::vector<pcl::PointXYZ> points;
    std::cout<<"Starting the project !";

    if (!file.is_open()) {
        std::cout<<"Unable to open the file !";
    }

    
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
            

        // std::cout<< "Token Size : "<<tokens.size()<< " ";
        if (tokens.size() == 4)
        {
            // std::cout<<"points vector initialized"<<std::endl;
            // std::cout<<"Token [0] : "<<tokens[0]<<std::endl;
            // std::cout<<"Token Type : "<<typeid(tokens[0]).name()<<std::endl;
            try {
                double x = std::stod(tokens[0]);
                double y = std::stod(tokens[1]);
                double z = std::stod(tokens[2]);
                pcl::PointXYZ point(x, y, z);
                points.push_back(point);
            }
            catch(...) {
                std::cout<<"Unable to parse the value"<<std::endl;
            }
            

            // std::cout<< "Value of X : "<< x << " "<<std::endl;
            // std::cout<<"Type of X : "<<typeid(x).name()<<std::endl;
            // std::cout<<tokens[1] << " "<<typeid(tokens[1]).name()<<std::endl;
            // std::cout<<tokens[2] << " "<<typeid(tokens[0]).name()<<std::endl;
            // float x = std::stof(tokens[0]);
            // float y = std::stof(tokens[1]);
            // float z = std::stof(tokens[2]);
            // pcl::PointXYZ point(x, y, z);
            // points.push_back(point);
        }
    }

    std::cout<<"The size of point cloud is : "<<points.size()<<std::endl;

    // Create a point cloud and populate it with the read points
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    cloud->width = points.size();
    cloud->height = 1;
    cloud->is_dense = false;
    cloud->points.resize(cloud->width * cloud->height);
    std::copy(points.begin(), points.end(), cloud->points.begin());

    // Save the point cloud as PCD file
    pcl::io::savePCDFileASCII(pcd_filename, *cloud);
    std::cout << "Saved " << cloud->size() << " data points to " << pcd_filename << std::endl;

    return 0;
}
