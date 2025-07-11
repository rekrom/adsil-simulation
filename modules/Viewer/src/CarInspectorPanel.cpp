#include "viewer/imgui/CarInspectorPanel.hpp"

namespace viewer::imgui
{

    void CarInspectorPanel::draw(const std::shared_ptr<CarEntity> &carEntity)
    {
        if (!carEntity->getCar() || !carEntity->getRenderable())
            return;
        if (ImGui::Begin("Car Inspector"))
        {
            drawTransformSection(carEntity->getCar());
            drawDimensionSection(carEntity);
            drawDeviceListSection(carEntity);
        }
        ImGui::End();
    }

    void CarInspectorPanel::drawTransformSection(const std::shared_ptr<Car> &car)
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Position
            Point pos = car->getPosition();
            float posArr[3] = {pos.x(), pos.y(), pos.z()};

            if (ImGui::DragFloat3("Position", posArr, 0.1F))
            {
                car->setPosition(Point(posArr[0], posArr[1], posArr[2]));
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset Pos"))
            {
                car->setPosition(Point(0.0F, 0.0F, 0.0F));
            }

            // Orientation
            Vector rpy = car->getOrientation();
            float rpyDeg[3] = {
                RotationUtils::rad2deg(rpy.x()),
                RotationUtils::rad2deg(rpy.y()),
                RotationUtils::rad2deg(rpy.z())};

            if (ImGui::DragFloat3("Orientation (Roll, Pitch, Yaw)", rpyDeg, 1.0F, -180.0F, 180.0F))
            {
                car->setOrientation(Vector(
                    RotationUtils::deg2rad(rpyDeg[0]),
                    RotationUtils::deg2rad(rpyDeg[1]),
                    RotationUtils::deg2rad(rpyDeg[2])));
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset Rot"))
            {
                car->setOrientation(Vector(0.0F, 0.0F, 0.0F));
            }
        }
    }

    void CarInspectorPanel::drawDimensionSection(const std::shared_ptr<viewer::CarEntity> &carEntity)
    {
        if (ImGui::CollapsingHeader("Dimension", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto car = carEntity->getCar();
            auto renderable = carEntity->getRenderable();

            CarDimension dim = car->getDimension();
            float size[3] = {dim.length, dim.width, dim.height};

            if (ImGui::DragFloat3("Size (L, W, H)", size, 0.01F, 0.0F, 1000.0F))
            {
                CarDimension newDim{size[0], size[1], size[2]};
                car->setDimension(newDim);
                renderable->rebuildMesh();
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset Size"))
            {
                car->setDimension(Car::DefaultCarDimension);
                renderable->rebuildMesh();
            }
        }
    }

    void CarInspectorPanel::drawDeviceListSection(const std::shared_ptr<viewer::CarEntity> &carEntity)
    {
        if (ImGui::CollapsingHeader("Devices", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::Checkbox("Show TX", &showTransmitters_))
            {
                for (auto &tx : carEntity->getTxEntities())
                    tx->setVisible(showTransmitters_);
            }

            if (ImGui::Checkbox("Show RX", &showReceivers_))
            {
                for (auto &rx : carEntity->getRxEntities())
                    rx->setVisible(showReceivers_);
            }

            if (showTransmitters_)
            {
                const auto &txs = carEntity->getTxEntities();
                if (ImGui::TreeNode("Transmitters"))
                {
                    for (size_t i = 0; i < txs.size(); ++i)
                    {
                        auto &txEntity = txs[i];
                        const auto &dev = txEntity->getDevice();
                        std::string label = dev->getName() + "##tx" + std::to_string(i);
                        if (ImGui::TreeNode(label.c_str()))
                        {
                            drawDeviceInspector(txEntity);
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }

            if (showReceivers_)
            {
                const auto &rxs = carEntity->getRxEntities();
                if (ImGui::TreeNode("Receivers"))
                {
                    for (size_t i = 0; i < rxs.size(); ++i)
                    {
                        auto &rxEntity = rxs[i];
                        const auto &dev = rxEntity->getDevice();
                        std::string label = dev->getName() + "##rx" + std::to_string(i);
                        if (ImGui::TreeNode(label.c_str()))
                        {
                            drawDeviceInspector(rxEntity);
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }
    }

    void CarInspectorPanel::drawDeviceInspector(const std::shared_ptr<DeviceEntity> &deviceEntity)
    {
        float hFov = deviceEntity->getDevice()->getHorizontalFovDeg();
        float vFov = deviceEntity->getDevice()->getVerticalFovDeg();

        if (ImGui::DragFloat("Horizontal FoV", &hFov, 1.0F, 1.0F, 180.0F))
        {
            deviceEntity->getDevice()->setHorizontalFovDeg(hFov);
        }

        if (ImGui::DragFloat("Vertical FoV", &vFov, 1.0F, 1.0F, 180.0F))
        {
            deviceEntity->getDevice()->setVerticalFovDeg(vFov);
        }

        Point pos = deviceEntity->getDevice()->getOrigin();
        float posArr[3] = {pos.x(), pos.y(), pos.z()};
        if (ImGui::DragFloat3("Position wrt car", posArr, 0.1F))
        {
            deviceEntity->getDevice()->setOrigin(Point(posArr[0], posArr[1], posArr[2]));
        }

        Vector rpy = deviceEntity->getDevice()->getOrientation();
        float rpyDeg[3] = {
            RotationUtils::rad2deg(rpy.x()),
            RotationUtils::rad2deg(rpy.y()),
            RotationUtils::rad2deg(rpy.z())};

        if (ImGui::DragFloat3("Orientation (RPY)", rpyDeg, 1.0F, -180.0F, 180.0F))
        {
            deviceEntity->getDevice()->setOrientation(Vector(
                RotationUtils::deg2rad(rpyDeg[0]),
                RotationUtils::deg2rad(rpyDeg[1]),
                RotationUtils::deg2rad(rpyDeg[2])));
        }

        float range = deviceEntity->getDevice()->getRange();

        if (ImGui::DragFloat("Range", &range, 1.0F, 1.0F, 50.0F))
            deviceEntity->getDevice()->setRange(range);

        static bool visible = true; // placeholder state
        if (ImGui::Checkbox("Visible", &visible))
        {
            deviceEntity->setVisible(visible);
        }
    }

} // namespace viewer
