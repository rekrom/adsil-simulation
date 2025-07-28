#include <viewer/entities/ShapeEntity.hpp>
#include <iostream> // temporary logging

namespace viewer
{

    ShapeEntity::ShapeEntity(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color)
        : shape_(shape),
          color_(color)
    {
        renderable_ = std::make_shared<ShapeRenderable>(shape_, color_);
    }

    glm::vec3 ShapeEntity::getCenter() const
    {
        if (shape_)
        {
            return renderable_->getCenter();
        }
        return glm::vec3(0.0F);
    }

    bool ShapeEntity::isTransparent() const
    {
        if (!renderable_)
        {
            return false;
        }
        return renderable_->isTransparent();
    }

}
