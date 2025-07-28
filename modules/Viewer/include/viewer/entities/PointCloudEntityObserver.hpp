#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "viewer/entities/Entity.hpp"
#include <math/PointCloud.hpp>
#include <viewer/renderables/PointCloudRenderable.hpp>
#include <simulation/interfaces/IFrameObserver.hpp>
#include <simulation/implementations/Frame.hpp>
#include <viewer/entities/PointCloudEntity.hpp>
#include <core/Logger.hpp>

namespace viewer
{

    class PointCloudEntityObserver : public simulation::IFrameObserver
    {
    public:
        PointCloudEntityObserver(std::shared_ptr<viewer::PointCloudEntity> entity)
            : entity_(entity)
        {
            observerName_ = "PointCloudEntityObserver";
        }

        void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override
        {
            LOGGER_INFO("PointCloudEntityObserver received frame change notification");
            if (entity_ && frame && frame->cloud)
            {
                entity_->setPointCloud(frame->cloud);
            }
        }

    private:
        std::shared_ptr<viewer::PointCloudEntity> entity_;
    };
}
