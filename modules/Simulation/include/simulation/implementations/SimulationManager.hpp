#pragma once

#include <filesystem>

#include <core/Car.hpp>
#include <core/ResourceLocator.hpp>
#include <simulation/SimulationScene.hpp>
#include <viewer/implementations/OpenGLViewer.hpp>
#include <viewer/entities/entities.hpp>
#include <simulation/implementations/InputManager.hpp>
#include <adapter/AdapterManager.hpp>
#include <simulation/SignalSolver.hpp>

namespace simulation
{

    class SimulationManager
    {
    public:
        void init();
        void update(float deltaTime);
        void render();
        void run(); // main loop

    private:
        void createEntities();

    private:
        std::unique_ptr<viewer::OpenGLViewer> viewer_;
        std::shared_ptr<simulation::InputManager> inputManager_;
        std::unique_ptr<adapter::AdapterManager> adapters_;
        std::shared_ptr<SimulationScene> scene_;
        std::unique_ptr<SignalSolver> signalSolver_;

        // Future: simulation time, sensor updates, etc.
    };

} // namespace simulation
