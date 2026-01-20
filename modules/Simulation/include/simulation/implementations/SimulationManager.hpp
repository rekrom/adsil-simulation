#pragma once

#include <filesystem>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <atomic>
#include <vehicle/Car.hpp>
#include <viewer/interfaces/IViewer.hpp>
#include <viewer/entities/entities.hpp>
#include <viewer/entities/PointCloudEntityObserver.hpp>
#include <viewer/implementations/OpenGLViewer.hpp>
#include <viewer/entities/GroundEntity.hpp>
#include <viewer/entities/AxisEntity.hpp>
#include <viewer/entities/PointCloudEntity.hpp>
#include <simulation/SimulationScene.hpp>
#include <simulation/interfaces/ISimulationScene.hpp>
#include <simulation/implementations/InputManager.hpp>
#include <simulation/implementations/FrameBufferManager.hpp>
#include <simulation/SignalSolver.hpp>
#include <simulation/interfaces/ISolver.hpp>
#include <simulation/interfaces/IFrameObserver.hpp>
#include <simulation/configs/SimulationConfig.hpp>
#include <core/Logger.hpp>
#include <core/ResourceLocator.hpp>
#include <core/Timer.hpp>
#include <adapter/AdapterManager.hpp>
#include <utils/DataExporter.hpp>

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

        // Viewer injection (for testing or alternative implementations)
        void setViewer(std::unique_ptr<viewer::IViewer> viewer) { viewer_ = std::move(viewer); }

        // Performance monitoring
        void reportPerformanceStats() const;
        void resetPerformanceStats();

    private:
        void createEntities();
        void initializeComponents();
        void validateEssentialComponents();
        // Extracted internal helper for signal solving & detected point cloud update
        void processSignals_();
        // Apply pending point cloud from async processing
        void applyPendingPointCloud_();

    private:
        // Configuration
        std::shared_ptr<SimulationConfig> config_;

        // Core components
        std::unique_ptr<viewer::IViewer> viewer_;
        std::shared_ptr<FrameBufferManager> frameBuffer_;
        bool hasFrameChanged_ = false;
        std::shared_ptr<simulation::InputManager> inputManager_;
        std::unique_ptr<adapter::AdapterManager> adapters_;
        std::shared_ptr<SimulationScene> scene_;
        std::shared_ptr<simulation::ISolver> signalSolver_;

        // Entities
        std::shared_ptr<viewer::PointCloudEntity> pcEntity_;
        std::shared_ptr<viewer::PointCloudEntity> detectedPointCloudEntity_;

        // Async signal processing state
        std::atomic<bool> signalProcessingBusy_{false};
        std::mutex pendingResultMutex_;
        std::shared_ptr<math::PointCloud> pendingPointCloud_;
        std::atomic<bool> hasPendingResult_{false};
    };

} // namespace simulation
