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
        j["imu"]["linear_acceleration"] = frame->linearAcceleration;
        j["imu"]["angular_velocity"] = frame->angularVelocity;

        j["pointcloud"] = nlohmann::json::array();
        for (const auto &pt : frame->cloud->getPoints())
        {
            j["pointcloud"].push_back({pt.x(), pt.y(), pt.z()});
        }

        return j;
    }

    std::shared_ptr<simulation::Frame> FrameJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        auto cloud = std::make_shared<PointCloud>();
        for (const auto &pt : j["pointcloud"])
        {
            if (pt.size() == 3)
                cloud->addPoint(Point(pt[0], pt[1], pt[2]));
        }

        std::shared_ptr<simulation::Frame> frame;
        frame->cloud = cloud;
        frame->timestamp = j["timestamp"];
        frame->linearAcceleration = j["imu"]["linear_acceleration"].get<std::vector<float>>();
        frame->angularVelocity = j["imu"]["angular_velocity"].get<std::vector<float>>();
        return frame;
    }
};
