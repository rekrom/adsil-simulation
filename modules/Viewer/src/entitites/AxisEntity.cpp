// AxisEntity.cpp
#include <viewer/entities/AxisEntity.hpp>
#include <iostream>
namespace viewer
{
    AxisEntity::AxisEntity()
        : renderable_(std::make_shared<AxisRenderable>())
    {
    }

    void AxisEntity::initGL()
    {
        if (!renderable_)
        {
            LOGGER_ERROR("AxisEntity::initGL: renderable not found");
        }

        renderable_->initGL();
    }

    void AxisEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!isVisible() || !renderable_)
        {
            return;
        }
        renderable_->render(view, projection);
    }

    void AxisEntity::cleanup()
    {
        if (!renderable_)
        {
            return;
        }
        renderable_->cleanup();
    }
}
