#include <viewer/entities/ShapeEntity.hpp>
#include <iostream> // temporary logging

namespace viewer
{

    ShapeEntity::ShapeEntity(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color) : shape_(shape), renderable_(std::make_shared<ShapeRenderable>(shape_, color_)), color_(color)
    {
    }

    void ShapeEntity::initGL()
    {
        renderable_->initGL(); // likely crash happens here
    }

    void ShapeEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        renderable_->render(view, projection);
    }

    void ShapeEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();
    }

    glm::vec3 ShapeEntity::getColor() const
    {
        return color_;
    }

    glm::vec3 ShapeEntity::getCenter() const
    {
        if (shape_)
        {
            return renderable_->getCenter();
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
