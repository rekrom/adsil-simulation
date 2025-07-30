#pragma once

#include <memory>
#include <glm/vec3.hpp>

#include <geometry/implementations/Device.hpp>
#include <viewer/renderables/DeviceRenderable.hpp>
#include <viewer/entities/Entity.hpp>

namespace viewer
{
    class DeviceEntity : public Entity
    {
    public:
        explicit DeviceEntity(std::shared_ptr<Device> device, glm::vec3 color = {1.0F, 1.0F, 1.0F}, bool visible = true);

        std::shared_ptr<Device> getDevice() const;

        void setFoVVisible(bool visible);
        void setFovRenderableColor(glm::vec3 color);
        glm::vec3 getFovRenderableColor() const;
        void setColor(glm::vec3 color);

    private:
        std::shared_ptr<Device> device_;
        glm::vec3 color_;
        bool visible_;
    };
}
