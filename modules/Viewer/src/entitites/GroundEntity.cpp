// GroundEntity.cpp
#include <viewer/entities/GroundEntity.hpp>

namespace viewer
{
    GroundEntity::GroundEntity()
        : renderable_(std::make_shared<GroundRenderable>())
    {
    }

    void GroundEntity::initGL()
    {
        renderable_->initGL();
    }

    void GroundEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!visible_ || !renderable_)
            return;
        renderable_->render(view, projection);
    }

    void GroundEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();
    }
}
