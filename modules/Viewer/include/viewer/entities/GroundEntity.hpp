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

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        std::string getName() const override { return "Ground"; }
        bool isTransparent() const override { return renderable_ && renderable_->isTransparent(); }
        glm::vec3 getCenter() const override { return renderable_ ? renderable_->getCenter() : glm::vec3(0.0F); }

    private:
        std::shared_ptr<GroundRenderable> renderable_;
    };
}
