#include <simulation/implementations/PointCloudScene.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <math/Point.hpp>

using json = nlohmann::json;

PointCloudScene::PointCloudScene(const std::string &jsonPath)
    : cloud_(std::make_shared<math::PointCloud>()), timestamp_(0.0)
{
    std::ifstream in(jsonPath);
    if (!in.is_open())
    {
        throw std::runtime_error("Could not open point cloud JSON file: " + jsonPath);
    }

    json j;
    in >> j;

    if (j.contains("timestamp"))
    {
        timestamp_ = j["timestamp"].get<double>();
    }

    if (j.contains("pointcloud") && j["pointcloud"].is_array())
    {
        for (const auto &pt : j["pointcloud"])
        {
            if (pt.is_array() && pt.size() == 3)
            {
                cloud_->addPoint(math::Point(
                    static_cast<float>(pt[0]),
                    static_cast<float>(pt[1]),
                    static_cast<float>(pt[2])));
            }
        }
    }
}

std::shared_ptr<math::PointCloud> PointCloudScene::getMergedPointCloud(int) const
{
    return cloud_;
}

double PointCloudScene::getTimestamp() const
{
    return timestamp_;
}
