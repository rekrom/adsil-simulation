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
#include <simulation/configs/SimulationConfig.hpp>
#include <core/Timer.hpp>

namespace simulation
{

    class SimulationManager : public simulation::IFrameObserver, public std::enable_shared_from_this<SimulationManager>
    {
    public:
        // Constructors
        SimulationManager();
        explicit SimulationManager(std::shared_ptr<SimulationConfig> config);

        // Core lifecycle methods
        void init();
        void update(float deltaTime);
        void render();
        void run(); // main loop

        // Simulation control methods
        void play();
        void pause();
        void seek(int frameIndex);

        // Frame observer interface
        void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override;

        // Configuration
        void setConfig(std::shared_ptr<SimulationConfig> config) { config_ = config; }
        std::shared_ptr<SimulationConfig> getConfig() const { return config_; }

        // Performance monitoring
        void reportPerformanceStats() const;
        void resetPerformanceStats();

    private:
        void createEntities();
        void initializeComponents();
        void validateEssentialComponents();

    private:
        // Configuration
        std::shared_ptr<SimulationConfig> config_;

        // Core components
        std::unique_ptr<viewer::OpenGLViewer> viewer_;
        std::shared_ptr<FrameBufferManager> frameBuffer_;
        std::shared_ptr<simulation::InputManager> inputManager_;
        std::unique_ptr<adapter::AdapterManager> adapters_;
        std::shared_ptr<SimulationScene> scene_;
        std::shared_ptr<SignalSolver> signalSolver_;

        // Entities
        std::shared_ptr<viewer::PointCloudEntity> pcEntity_;
        std::shared_ptr<viewer::PointCloudEntity> detectedPointCloudEntity_;
    };

} // namespace simulation
