#pragma once

#include <filesystem>

#include <vehicle/Car.hpp>
#include <core/ResourceLocator.hpp>
#include <simulation/SimulationScene.hpp>
#include <simulation/interfaces/ISimulationScene.hpp>
#include <viewer/implementations/OpenGLViewer.hpp>
#include <viewer/entities/entities.hpp>
#include <simulation/implementations/InputManager.hpp>
#include <simulation/implementations/FrameBufferManager.hpp>
#include <adapter/AdapterManager.hpp>
#include <simulation/SignalSolver.hpp>
#include <core/Logger.hpp>
#include <viewer/entities/PointCloudEntityObserver.hpp>
#include <simulation/interfaces/IFrameObserver.hpp>

namespace simulation
{

    class SimulationManager : public simulation::IFrameObserver, public std::enable_shared_from_this<SimulationManager>
    {
    public:
        void init();
        void update(float deltaTime);
        void render();
        void run(); // main loop

        void play();
        void pause();
        void seek(int frameIndex);

        void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override;

    private:
        void createEntities();

    private:
        std::unique_ptr<viewer::OpenGLViewer> viewer_;
        std::shared_ptr<FrameBufferManager> frameBuffer_;
        std::shared_ptr<simulation::InputManager> inputManager_;
        std::unique_ptr<adapter::AdapterManager> adapters_;
        std::shared_ptr<SimulationScene> scene_;
        // std::shared_ptr<ISimulationScene> scene_;
        std::shared_ptr<SignalSolver> signalSolver_;

        std::shared_ptr<viewer::PointCloudEntity> pcEntity_;
        std::shared_ptr<viewer::PointCloudEntity> detectedPointCloudEntity_;
    };

} // namespace simulation
