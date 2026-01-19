#pragma once

#include <memory>
#include <vector>
#include "imgui.h"
#include <vehicle/Car.hpp>
#include <geometry/implementations/Device.hpp>
#include <math/RotationUtils.hpp>
#include <viewer/renderables/CarRenderable.hpp>
#include <viewer/entities/CarEntity.hpp>
#include <viewer/entities/DeviceEntity.hpp>

namespace viewer::imgui
{
    class CarInspectorPanel
    {
    public:
        // void draw(const std::shared_ptr<Car> &car, const std::shared_ptr<CarRenderable> &carRenderable);
        void draw(const std::shared_ptr<CarEntity> &carEntity);

    private:
        void drawTransformSection(const std::shared_ptr<Car> &car);
        void drawDimensionSection(const std::shared_ptr<CarEntity> &carEntity);
        void drawDeviceListSection(const std::shared_ptr<viewer::CarEntity> &carEntity);
        void drawDeviceInspector(const std::shared_ptr<DeviceEntity> &deviceEntity);

        bool showTransmitters_ = true;
        bool showReceivers_ = true;
        float txRange_ = 5.0F;
        float rxRange_ = 5.0F;
    };

} // namespace viewer
