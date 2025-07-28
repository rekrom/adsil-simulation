#include <viewer/entities/DeviceEntity.hpp>
#include <iostream>
namespace viewer
{
    DeviceEntity::DeviceEntity(std::shared_ptr<Device> device, glm::vec3 color, bool visible)
        : device_(std::move(device)), color_(color), visible_(visible)
    {
        renderable_ = std::make_shared<DeviceRenderable>(device_, color_);
        setName("DeviceEntity - " + device_->getName());
    }

    std::shared_ptr<Device> DeviceEntity::getDevice() const
    {
        return device_;
    }

    void DeviceEntity::setFoVVisible(bool visible)
    {
        if (renderable_)
            std::dynamic_pointer_cast<DeviceRenderable>(renderable_)->enableFoV(visible);
    }

    void DeviceEntity::setColor(glm::vec3 color)
    {
        color_ = color;
        if (renderable_)
        {
            std::dynamic_pointer_cast<DeviceRenderable>(renderable_)->setColor(color_);
        }
    }

    void DeviceEntity::setFovRenderableColor(glm::vec3 color)
    {
        std::dynamic_pointer_cast<DeviceRenderable>(renderable_)->setFovPyramidColor(color);
    }
    glm::vec3 DeviceEntity::getFovRenderableColor() const
    {
        return std::dynamic_pointer_cast<DeviceRenderable>(renderable_)->getFovPyramidColor();
    }

}
