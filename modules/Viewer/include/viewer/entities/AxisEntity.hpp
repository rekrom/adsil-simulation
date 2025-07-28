// GroundEntity.hpp
#pragma once

#include <memory>
#include <viewer/entities/Entity.hpp>
#include <viewer/renderables/AxisRenderable.hpp>
#include <core/Logger.hpp>

namespace viewer
{
    class AxisEntity : public Entity
    {
    public:
        AxisEntity();

        // std::string getName() const override { return "Axis"; }
        bool isTransparent() const override { return renderable_ && renderable_->isTransparent(); }
        glm::vec3 getCenter() const override { return renderable_ ? renderable_->getCenter() : glm::vec3(0.0F); }
        std::string getName() const override { return "Axis"; }
    };
}
