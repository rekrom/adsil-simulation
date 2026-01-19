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
        if (ImGui::CollapsingHeader("Car Global Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Position
            auto node = car->getTransformNode();
            spatial::Transform transform = node->getLocalTransform();
            float posArr[3] = {
                transform.getPosition().x(),
                transform.getPosition().y(),
                transform.getPosition().z()};

            if (ImGui::DragFloat3("Position", posArr, 0.1F))
            {
                transform.setPosition(Point(posArr[0], posArr[1], posArr[2]));
                node->setLocalTransform(transform);
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset Pos"))
            {
                transform.setPosition(Point(0.0F, 0.0F, 0.0F));
                node->setLocalTransform(transform);
            }

            // Orientation
            Vector rpy = transform.getOrientation(); // Use the same 'transform' from earlier
            float rpyDeg[3] = {
                math::RotationUtils::rad2deg(rpy.x()),
                math::RotationUtils::rad2deg(rpy.y()),
                math::RotationUtils::rad2deg(rpy.z())};

            if (ImGui::DragFloat3("Orientation (RotateX, RotateY, RotateZ)", rpyDeg, 1.0F, -180.0F, 180.0F))
            {
                transform.setOrientation(math::Vector(
                    math::RotationUtils::deg2rad(rpyDeg[0]),
                    math::RotationUtils::deg2rad(rpyDeg[1]),
                    math::RotationUtils::deg2rad(rpyDeg[2])));
                node->setLocalTransform(transform);
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset Rot"))
            {
                transform.setOrientation(Vector(0.0F, 0.0F, 0.0F));
                node->setLocalTransform(transform);
            }
        }
    }

    void CarInspectorPanel::drawDimensionSection(const std::shared_ptr<viewer::CarEntity> &carEntity)
    {
        if (ImGui::CollapsingHeader("Dimension", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto car = carEntity->getCar();
            std::shared_ptr<CarRenderable> renderable = std::dynamic_pointer_cast<CarRenderable>(carEntity->getRenderable());

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
            // Visibility toggles on same line
            if (ImGui::Checkbox("Show TX", &showTransmitters_))
            {
                for (auto &tx : carEntity->getTxEntities())
                    tx->setVisible(showTransmitters_);
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Show RX", &showReceivers_))
            {
                for (auto &rx : carEntity->getRxEntities())
                    rx->setVisible(showReceivers_);
            }

            ImGui::Separator();

            // Transmitters section
            const auto &txs = carEntity->getTxEntities();
            if (ImGui::TreeNode("Transmitters"))
            {
                // Range control for all transmitters
                if (ImGui::DragFloat("Range (All)##tx", &txRange_, 1.0F, 1.0F, 100.0F))
                {
                    for (auto &tx : txs)
                        tx->getDevice()->setRange(txRange_);
                }

                if (showTransmitters_)
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
                }
                ImGui::TreePop();
            }

            // Receivers section
            const auto &rxs = carEntity->getRxEntities();
            if (ImGui::TreeNode("Receivers"))
            {
                // Range control for all receivers
                if (ImGui::DragFloat("Range (All)##rx", &rxRange_, 1.0F, 1.0F, 100.0F))
                {
                    for (auto &rx : rxs)
                        rx->getDevice()->setRange(rxRange_);
                }

                if (showReceivers_)
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
                }
                ImGui::TreePop();
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

        auto node = deviceEntity->getDevice()->getTransformNode();
        spatial::Transform transform = node->getLocalTransform();

        // Position
        float posArr[3] = {
            transform.getPosition().x(),
            transform.getPosition().y(),
            transform.getPosition().z()};

        if (ImGui::DragFloat3("Position wrt car", posArr, 0.1F))
        {
            transform.setPosition(Point(posArr[0], posArr[1], posArr[2]));
            node->setLocalTransform(transform);
        }

        // Orientation
        Vector rpy = transform.getOrientation();
        float rpyDeg[3] = {
            math::RotationUtils::rad2deg(rpy.x()),
            math::RotationUtils::rad2deg(rpy.y()),
            math::RotationUtils::rad2deg(rpy.z())};

        if (ImGui::DragFloat3("Orientation (RotateX, RotateY, RotateZ)", rpyDeg, 1.0F, -180.0F, 180.0F))
        {
            transform.setOrientation(math::Vector(
                math::RotationUtils::deg2rad(rpyDeg[0]),
                math::RotationUtils::deg2rad(rpyDeg[1]),
                math::RotationUtils::deg2rad(rpyDeg[2])));
            node->setLocalTransform(transform);
        }

        float range = deviceEntity->getDevice()->getRange();

        if (ImGui::DragFloat("Range", &range, 1.0F, 1.0F, 50.0F))
            deviceEntity->getDevice()->setRange(range);

        bool visible = deviceEntity->isVisible();
        if (ImGui::Checkbox("Visible", &visible))
        {
            deviceEntity->setVisible(visible);
        }
    }

} // namespace viewer
