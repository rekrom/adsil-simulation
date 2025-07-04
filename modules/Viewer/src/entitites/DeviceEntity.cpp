#include <viewer/entities/DeviceEntity.hpp>

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
        if (renderable_)
            renderable_->initGL();
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
}
