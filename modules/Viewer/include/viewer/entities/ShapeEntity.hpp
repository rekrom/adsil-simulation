#pragma once

#include <geometry/implementations/ShapeBase.hpp>
#include <viewer/entities/entities.hpp>
#include <viewer/renderables/ShapeRenderable.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace viewer
{

    class ShapeEntity : public Entity
    {
    public:
        ShapeEntity(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color = glm::vec3(0.6F, 0.6F, 0.9F));

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        glm::vec3 getCenter() const override;
        bool isTransparent() const override;
        std::string getName() const override;

    private:
        std::shared_ptr<ShapeBase> shape_;
        std::shared_ptr<ShapeRenderable> renderable_;
        glm::vec3 color_;
    };

}
