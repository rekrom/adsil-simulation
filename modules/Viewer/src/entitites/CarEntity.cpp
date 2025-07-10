#include <viewer/entities/CarEntity.hpp>
#include <iostream>
namespace viewer
{
    CarEntity::CarEntity(std::shared_ptr<Car> car, glm::vec3 color)
        : car_(std::move(car)), renderable_(std::make_shared<CarRenderable>(car_, color)), color_(color)
    {

        for (const auto &tx : car_->getTransmitters())
        {
            std::cout << "tx entity" << std::endl;
            txEntities_.emplace_back(std::make_shared<DeviceEntity>(tx, glm::vec3(1.0F, 0.0F, 0.0F)));
        }

        for (const auto &rx : car_->getReceivers())
        {
            std::cout << "rx entity" << std::endl;
            rxEntities_.emplace_back(std::make_shared<DeviceEntity>(rx, glm::vec3(0.0F, 0.0F, 1.0F)));
        } // green for RX
    }

    void CarEntity::initGL()
    {
        // std::cout << "initGl started for [CarEntity]" << std::endl;

        if (renderable_)
        {
            renderable_->initGL();
        }
        else
            std::cout << "renderable not found for [CarEntity]" << std::endl;

        for (auto &tx : txEntities_)
        {
            tx->initGL();
            tx->setFovRenderableColor({0.7F, 0.1F, 0.1F});
        }

        for (auto &rx : rxEntities_)
        {
            rx->initGL();
            rx->setFovRenderableColor({0.1F, 0.1F, 0.7F});
        }
    }

    void CarEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (renderable_)
        {
            renderable_->render(view, projection);
        }
        for (auto &tx : txEntities_)
        {
            tx->getRenderable()->render(view, projection);
        }
        for (auto &rx : rxEntities_)
        {
            rx->getRenderable()->render(view, projection);
        }
    }

    void CarEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();

        for (auto &tx : txEntities_)
            tx->cleanup();

        for (auto &rx : rxEntities_)
            rx->cleanup();
    }

    bool CarEntity::isTransparent() const
    {
        return renderable_->isTransparent();
    }

    glm::vec3 CarEntity::getCenter() const
    {
        return renderable_->getCar()->getPosition().toGlmVec3();
    }

    std::shared_ptr<Car> CarEntity::getCar() const
    {
        return car_;
    }

    std::shared_ptr<CarRenderable> CarEntity::getRenderable() const
    {
        return renderable_;
    }

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
