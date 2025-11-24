#include <adapter/implementations/FrameJsonAdapter.hpp>
#include <iostream>

namespace adapter
{
    FrameJsonAdapter::FrameJsonAdapter()
    {
    }

    nlohmann::json FrameJsonAdapter::toJson(const std::shared_ptr<simulation::Frame> &frame) const
    {
        nlohmann::json j;
        j["timestamp"] = frame->timestamp;
        // j["imu"]["linear_acceleration"] = frame->linearAcceleration;
        // j["imu"]["angular_velocity"] = frame->angularVelocity;

        j["pointcloud"] = nlohmann::json::array();
        for (const auto &pt : frame->cloud->getPoints())
        {
            j["pointcloud"].push_back({pt.x(), pt.y(), pt.z()});
        }

        return j;
    }

    std::shared_ptr<simulation::Frame> FrameJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        auto cloud = std::make_shared<math::PointCloud>();
        for (const auto &pt : j.at("pointcloud"))
        {
            if (pt.size() == 3)
                cloud->addPoint(Point(pt.at(0).get<float>(), pt.at(1).get<float>(), pt.at(2).get<float>()));
        }

        std::shared_ptr<simulation::Frame> frame = std::make_shared<simulation::Frame>();
        frame->cloud = cloud;
        frame->timestamp = j.at("timestamp").get<double>();
        // frame->linearAcceleration = j.at("imu").at("linear_acceleration").get<std::vector<float>>();
        // frame->angularVelocity = j.at("imu").at("angular_velocity").get<std::vector<float>>();
        return frame;
    }
};
