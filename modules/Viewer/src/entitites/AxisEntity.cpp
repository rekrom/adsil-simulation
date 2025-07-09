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
        // std::cout << "initGl started for [AxisEntity]" << std::endl;

        if (renderable_)
            renderable_->initGL();
        else
            std::cout << "renderable not found for [AxisEntity]" << std::endl;
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
