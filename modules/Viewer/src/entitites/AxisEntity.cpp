// AxisEntity.cpp
#include <viewer/entities/AxisEntity.hpp>

namespace viewer
{
    AxisEntity::AxisEntity()
        : renderable_(std::make_shared<AxisRenderable>())
    {
    }

    void AxisEntity::initGL()
    {
        renderable_->initGL();
    }

    void AxisEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!visible_ || !renderable_)
            return;
        renderable_->render(view, projection);
    }

    void AxisEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();
    }
}
