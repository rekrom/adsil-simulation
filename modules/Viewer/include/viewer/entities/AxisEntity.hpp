// GroundEntity.hpp
#pragma once

#include <memory>
#include <viewer/entities/Entity.hpp>
#include <viewer/implementations/AxisRenderable.hpp>

namespace viewer
{
    class AxisEntity : public Entity
    {
    public:
        AxisEntity();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        std::string getName() const override { return "Axis"; }
        bool isTransparent() const override { return renderable_ && renderable_->isTransparent(); }
        glm::vec3 getCenter() const override { return renderable_ ? renderable_->getCenter() : glm::vec3(0.0f); }

    private:
        std::shared_ptr<AxisRenderable> renderable_;
    };
}
