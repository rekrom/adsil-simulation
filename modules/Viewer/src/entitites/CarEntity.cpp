#include <viewer/entities/CarEntity.hpp>
#include <iostream>
namespace viewer
{
    CarEntity::CarEntity(std::shared_ptr<Car> car, glm::vec3 color)
        : car_(std::move(car)), color_(color)
    {
        renderable_ = std::make_shared<CarRenderable>(car_, color_);
        setName("CarEntity - " + car_->getName());

        for (const auto &tx : car_->getTransmitters())
        {
            txEntities_.emplace_back(std::make_shared<DeviceEntity>(tx, glm::vec3(1.0F, 0.0F, 0.0F)));
        }

        for (const auto &rx : car_->getReceivers())
        {
            rxEntities_.emplace_back(std::make_shared<DeviceEntity>(rx, glm::vec3(0.0F, 0.0F, 1.0F)));
        } // green for RX
    }

    std::shared_ptr<Car> CarEntity::getCar() const
    {
        return car_;
    }

    // std::shared_ptr<CarRenderable> CarEntity::getRenderable() const
    // {
    //     // return renderable_;
    // }

    const SharedVec<DeviceEntity> &CarEntity::getTxEntities() const
    {
        return txEntities_;
    }

    const SharedVec<DeviceEntity> &CarEntity::getRxEntities() const
    {
        return rxEntities_;
    }

    void CarEntity::setFoVVisible(bool enable)
    {
        for (auto &tx : txEntities_)
            tx->setFoVVisible(enable);
        for (auto &rx : rxEntities_)
            rx->setFoVVisible(enable);
    }

    void CarEntity::setColor(glm::vec3 newColor)
    {
        color_ = newColor;
        renderable_ = std::make_shared<CarRenderable>(car_, color_);
        renderable_->initGL(); // re-init with new color
    }
}
