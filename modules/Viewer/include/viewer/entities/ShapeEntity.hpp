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

    private:
        std::shared_ptr<ShapeBase> shape_;
        glm::vec3 color_;
    };

}
