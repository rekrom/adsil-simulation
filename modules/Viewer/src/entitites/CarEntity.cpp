#include <viewer/entities/CarEntity.hpp>

namespace viewer
{
    CarEntity::CarEntity(std::shared_ptr<Car> car, glm::vec3 color)
        : car_(std::move(car)), color_(color)
    {
        renderable_ = std::make_shared<CarRenderable>(car_, color_);

        for (const auto &tx : car_->getTransmitters())
            txEntities_.emplace_back(std::make_shared<DeviceEntity>(tx, glm::vec3(1.0f, 0.0f, 0.0f))); // red for TX

        for (const auto &rx : car_->getReceivers())
            rxEntities_.emplace_back(std::make_shared<DeviceEntity>(rx, glm::vec3(0.0f, 1.0f, 0.0f))); // green for RX
    }

    void CarEntity::initGL()
    {
        if (renderable_)
            renderable_->initGL();
        for (auto &tx : txEntities_)
            tx->initGL();
        for (auto &rx : rxEntities_)
            rx->initGL();
    }

    void CarEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (renderable_)
            renderable_->render(view, projection);
        for (auto &tx : txEntities_)
            tx->getRenderable()->render(view, projection);
        for (auto &rx : rxEntities_)
            rx->getRenderable()->render(view, projection);
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

    const std::vector<std::shared_ptr<DeviceEntity>> &CarEntity::getTxEntities() const
    {
        return txEntities_;
    }

    const std::vector<std::shared_ptr<DeviceEntity>> &CarEntity::getRxEntities() const
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
