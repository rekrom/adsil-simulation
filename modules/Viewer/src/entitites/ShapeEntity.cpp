#include <viewer/entities/ShapeEntity.hpp>
#include <iostream> // temporary logging

namespace viewer
{

    ShapeEntity::ShapeEntity(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color)
        : shape_(shape),
          color_(color)
    {
        renderable_ = std::make_shared<ShapeRenderable>(shape_, color_);
        setName("ShapeEntity");
    }

}
