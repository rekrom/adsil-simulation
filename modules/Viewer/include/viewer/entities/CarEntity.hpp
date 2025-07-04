#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <core/Car.hpp>
#include <viewer/implementations/CarRenderable.hpp>
#include <viewer/entities/DeviceEntity.hpp>
#include <viewer/entities/Entity.hpp>

namespace viewer
{
    class CarEntity : public Entity
    {
    public:
        explicit CarEntity(std::shared_ptr<Car> car, glm::vec3 color = {0.2f, 0.6f, 1.0f});

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        glm::vec3 getColor() const override { return color_; }

        std::string getName() const override
        {
            return car_ ? car_->getName() : "Unnamed Car";
        }

        std::shared_ptr<Car> getCar() const;
        std::shared_ptr<CarRenderable> getRenderable() const;

        const std::vector<std::shared_ptr<DeviceEntity>> &getTxEntities() const;
        const std::vector<std::shared_ptr<DeviceEntity>> &getRxEntities() const;

        void setFoVVisible(bool enable);
        void setColor(glm::vec3 newColor); // Also updates device colors

    private:
        std::shared_ptr<Car> car_;
        std::shared_ptr<CarRenderable> renderable_;
        std::vector<std::shared_ptr<DeviceEntity>> txEntities_;
        std::vector<std::shared_ptr<DeviceEntity>> rxEntities_;
        glm::vec3 color_;
    };
}
