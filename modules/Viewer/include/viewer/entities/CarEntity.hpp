#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <vehicle/Car.hpp>
#include <viewer/renderables/CarRenderable.hpp>
#include <viewer/entities/DeviceEntity.hpp>
#include <viewer/entities/Entity.hpp>

namespace viewer
{
    class CarEntity : public Entity
    {
    public:
        explicit CarEntity(std::shared_ptr<Car> car, glm::vec3 color = {0.2F, 0.6F, 1.0F});

        glm::vec3 getCenter() const override;
        std::string getName() const override { return car_ ? car_->getName() : "CarEntity"; }

        bool isTransparent() const override;

        std::shared_ptr<Car> getCar() const;
        // std::shared_ptr<CarRenderable> getRenderable() const;

        const SharedVec<DeviceEntity> &getTxEntities() const;
        const SharedVec<DeviceEntity> &getRxEntities() const;

        void setFoVVisible(bool enable);
        void setColor(glm::vec3 newColor); // Also updates device colors

    private:
        std::shared_ptr<Car> car_;
        SharedVec<DeviceEntity> txEntities_;
        SharedVec<DeviceEntity> rxEntities_;
        glm::vec3 color_;
    };
}
