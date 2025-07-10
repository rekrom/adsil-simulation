#include <viewer/entities/DeviceEntity.hpp>
#include <iostream>
namespace viewer
{
    DeviceEntity::DeviceEntity(std::shared_ptr<Device> device, glm::vec3 color, bool visible)
        : device_(std::move(device)), color_(color), visible_(visible)
    {
        renderable_ = std::make_shared<DeviceRenderable>(device_, color_);
    }

    std::shared_ptr<Device> DeviceEntity::getDevice() const
    {
        return device_;
    }

    std::shared_ptr<DeviceRenderable> DeviceEntity::getRenderable() const
    {
        return renderable_;
    }

    void DeviceEntity::initGL()
    {
        // std::cout << "initGl started for [DeviceEntity]" << std::endl;

        if (renderable_)
            renderable_->initGL();
        else
            std::cout << "renderable not found for [DeviceEntity]" << std::endl;
    }

    void DeviceEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();
    }

    void DeviceEntity::setFoVVisible(bool visible)
    {
        if (renderable_)
            renderable_->enableFoV(visible);
    }

    void DeviceEntity::setColor(glm::vec3 color)
    {
        std::cout << "[devEntity] setColor called" << std::endl;
        color_ = color;
        renderable_ = std::make_shared<DeviceRenderable>(device_, color_);
        renderable_->initGL(); // Re-init GL with new color
    }

    void DeviceEntity::setVisible(bool visible)
    {
        visible_ = visible;
    }
    bool DeviceEntity::getVisible()
    {
        return visible_;
    }

    void DeviceEntity::setFovRenderableColor(glm::vec3 color)
    {
        renderable_->setFovPyramidColor(color);
    }
    glm::vec3 DeviceEntity::getFovRenderableColor() const
    {
        return renderable_->getFovPyramidColor();
    }

}
