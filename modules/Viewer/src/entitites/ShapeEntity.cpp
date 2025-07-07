#include <viewer/entities/ShapeEntity.hpp>
#include <iostream> // temporary logging

namespace viewer
{

    ShapeEntity::ShapeEntity(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color) : shape_(shape), color_(color)
    {
    }

    void ShapeEntity::initGL()
    {
        shapeRenderable_ = std::make_unique<ShapeRenderable>(shape_, color_);
        shapeRenderable_->initGL(); // likely crash happens here
    }

    void ShapeEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        shapeRenderable_->render(view, projection);
    }

    void ShapeEntity::cleanup()
    {
        shapeRenderable_->cleanup();
    }

    glm::vec3 ShapeEntity::getColor() const
    {
        return color_;
    }

    glm::vec3 ShapeEntity::getCenter() const
    {
        if (shape_)
        {
            return shapeRenderable_->getCenter();
        }
        return glm::vec3(0.0f);
    }

    bool ShapeEntity::isTransparent() const
    {
        return false; // For now
    }

    std::string ShapeEntity::getName() const
    {
        return shape_->getName();
    }

}
