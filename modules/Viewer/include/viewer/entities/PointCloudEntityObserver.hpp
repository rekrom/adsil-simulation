#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "viewer/entities/Entity.hpp"
#include "core/PointCloud.hpp"
#include "viewer/implementations/PointCloudRenderable.hpp"
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
            : entity_(entity) {}

        void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override
        {
            if (entity_ && frame && frame->cloud)
                entity_->setPointCloud(frame->cloud);
        }

    private:
        std::shared_ptr<viewer::PointCloudEntity> entity_;
    };
}
