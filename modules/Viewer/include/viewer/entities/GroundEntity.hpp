// GroundEntity.hpp
#pragma once

#include <memory>
#include <viewer/entities/Entity.hpp>
#include <viewer/renderables/GroundRenderable.hpp>

namespace viewer
{
    class GroundEntity : public Entity
    {
    public:
        GroundEntity();

        glm::vec3 getCenter() const override { return renderable_ ? renderable_->getCenter() : glm::vec3(0.0F); }
        std::string getName() const override { return "GroundEntity"; }
    };
}
