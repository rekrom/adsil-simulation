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

namespace simulation
{

    class SimulationManager
    {
    public:
        void init();
        void update(float deltaTime);
        void render();
        void run(); // main loop

        void play();
        void pause();
        void seek(int frameIndex);

    private:
        void createEntities();

    private:
        std::unique_ptr<viewer::OpenGLViewer> viewer_;
        std::shared_ptr<FrameBufferManager> frameBuffer_;
        std::shared_ptr<simulation::InputManager> inputManager_;
        std::unique_ptr<adapter::AdapterManager> adapters_;
        std::shared_ptr<SimulationScene> scene_;
        // std::shared_ptr<ISimulationScene> scene_;
        std::unique_ptr<SignalSolver> signalSolver_;

        std::shared_ptr<viewer::PointCloudEntity> detectedPointCloudEntity_;

        // In SimulationManager.hpp:
        std::shared_ptr<viewer::PointCloudEntityObserver> pcEntityObserver_;
        // Future: simulation time, sensor updates, etc.3
    };

} // namespace simulation
