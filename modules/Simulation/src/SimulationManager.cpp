#include <simulation/implementations/SimulationManager.hpp>
#include <viewer/entities/GroundEntity.hpp>
#include <viewer/entities/AxisEntity.hpp>
#include <viewer/entities/PointCloudEntity.hpp>
#include <core/Timer.hpp>
#include <stdexcept>

// Fallback base resource directory if environment variable is not set
#ifndef FALLBACK_RESOURCE_DIR
#define FALLBACK_RESOURCE_DIR "/home/rkrm-dev/Desktop/adsil_analyzer_cpp/resources"
#endif

namespace simulation
{
    SimulationManager::SimulationManager()
        : config_(SimulationConfig::createDefault())
    {
    }

    SimulationManager::SimulationManager(std::shared_ptr<SimulationConfig> config)
        : config_(config)
    {
        if (!config_)
        {
            config_ = SimulationConfig::createDefault();
        }
    }

    void SimulationManager::init()
    {
        // Set observer name for debugging
        observerName_ = "SimulationManagerObserver";

        try
        {
            initializeComponents();
            validateEssentialComponents();
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("SimulationManager initialization failed: ") + e.what());
            throw;
        }
    }

    void SimulationManager::initializeComponents()
    {
        const auto &resourceConfig = config_->getResourceConfig();
        const auto &frameConfig = config_->getFrameConfig();

        // Set the base path for resolving resources
        core::ResourceLocator::setBasePath(resourceConfig.basePath);

        // Initialize adapter system
        adapters_ = std::make_unique<adapter::AdapterManager>();

        // Load the simulation scene (includes car, devices, etc.)
        scene_ = adapters_->fromJson<std::shared_ptr<SimulationScene>>(
            core::ResourceLocator::getJsonPath(resourceConfig.sceneFile));

        // Initialize frame buffer with configured window size
        frameBuffer_ = std::make_shared<FrameBufferManager>(frameConfig.bufferWindowSize);

        // Initialize the OpenGL viewer with configured parameters
        const auto &windowConfig = config_->getWindowConfig();
        viewer_ = std::make_unique<viewer::OpenGLViewer>(
            windowConfig.width,
            windowConfig.height,
            windowConfig.title);
        viewer_->setFrameManager(frameBuffer_);

        // Initialize the input manager (for controlling the simulation)
        // Pass the viewer's input manager to create proper separation of concerns
        inputManager_ = std::make_shared<simulation::InputManager>(viewer_->getInputManager());

        // Initialize the signal solver (sensor signal processing, etc.)
        signalSolver_ = std::make_shared<SignalSolver>(scene_);

        // Register this manager as a frame observer
        frameBuffer_->addFrameObserver(shared_from_this());
    }

    void SimulationManager::validateEssentialComponents()
    {
        if (!scene_)
        {
            throw std::runtime_error("Failed to load simulation scene from JSON");
        }

        if (!scene_->getCar())
        {
            throw std::runtime_error("Scene is missing essential car component");
        }

        if (!viewer_)
        {
            throw std::runtime_error("Failed to initialize OpenGL viewer");
        }

        if (!frameBuffer_)
        {
            throw std::runtime_error("Failed to initialize frame buffer manager");
        }

        if (!signalSolver_)
        {
            throw std::runtime_error("Failed to initialize signal solver");
        }
    }

    void SimulationManager::createEntities()
    {
        if (!scene_)
        {
            LOGGER_ERROR("Cannot create entities: scene is null");
            return;
        }

        const auto &carConfig = config_->getCarConfig();
        const auto &pointCloudConfig = config_->getPointCloudConfig();

        SharedVec<viewer::Entity> entities;

        // Create basic scene entities
        auto axisEntity = std::make_shared<viewer::AxisEntity>();
        // auto groundEntity = std::make_shared<viewer::GroundEntity>();
        auto carEntity = std::make_shared<viewer::CarEntity>(scene_->getCar(), carConfig.color);

        entities.push_back(axisEntity);
        // entities.push_back(groundEntity);
        entities.push_back(carEntity);

        // Add vehicle device entities
        auto rxDevices = carEntity->getRxEntities();
        auto txDevices = carEntity->getTxEntities();

        for (const auto &tx : txDevices)
        {
            entities.push_back(tx);
        }
        for (const auto &rx : rxDevices)
        {
            entities.push_back(rx);
        }

        // Add shape entities from scene
        for (auto shape : scene_->getShapes())
        {
            auto shapeEntity = std::make_shared<viewer::ShapeEntity>(shape);
            entities.push_back(shapeEntity);
        }

        // Create point cloud entities with configured parameters
        pcEntity_ = std::make_shared<viewer::PointCloudEntity>(scene_->getExternalPointCloud());
        pcEntity_->setName(pointCloudConfig.outsideName);
        entities.push_back(pcEntity_);

        detectedPointCloudEntity_ = std::make_shared<viewer::PointCloudEntity>();
        detectedPointCloudEntity_->setName(pointCloudConfig.detectedName);
        detectedPointCloudEntity_->setPointSize(pointCloudConfig.pointSize);
        detectedPointCloudEntity_->setAlpha(pointCloudConfig.alpha);
        detectedPointCloudEntity_->setColor(pointCloudConfig.detectedColor);

        entities.push_back(detectedPointCloudEntity_);

        // Set selected point cloud entity in viewer
        viewer_->setSelectedPointCloudEntity(detectedPointCloudEntity_);
        viewer_->setEntities(entities);
    }

    void SimulationManager::update(float deltaTime)
    {
        if (!inputManager_ || !scene_ || !scene_->getCar() || !viewer_ || !frameBuffer_)
        {
            LOGGER_ERROR("Cannot update: essential components are null");
            return;
        }

        try
        {
            inputManager_->processInput(deltaTime, *scene_->getCar(), viewer_->getCamera(), frameBuffer_);
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Error during update: ") + e.what());
        }
    }

    void SimulationManager::render()
    {
        if (!viewer_)
        {
            LOGGER_ERROR("Cannot render: viewer is null");
            return;
        }

        try
        {
            viewer_->render();
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Error during rendering: ") + e.what());
        }
    }

    void SimulationManager::play()
    {
        if (!frameBuffer_)
        {
            LOGGER_ERROR("Cannot play: frame buffer is null");
            return;
        }

        try
        {
            frameBuffer_->play();
            LOGGER_INFO("Simulation playback started");
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Error starting playback: ") + e.what());
        }
    }

    void SimulationManager::pause()
    {
        if (!frameBuffer_)
        {
            LOGGER_ERROR("Cannot pause: frame buffer is null");
            return;
        }

        try
        {
            frameBuffer_->pause();
            LOGGER_INFO("Simulation playback paused");
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Error pausing playback: ") + e.what());
        }
    }

    void SimulationManager::seek(int frameIndex)
    {
        if (!frameBuffer_)
        {
            LOGGER_ERROR("Cannot seek: frame buffer is null");
            return;
        }

        try
        {
            frameBuffer_->seek(frameIndex);
            LOGGER_INFO(std::string("Seeking to frame ") + std::to_string(frameIndex));
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Error seeking to frame ") + std::to_string(frameIndex) + ": " + e.what());
        }
    }

    void SimulationManager::run()
    {
        try
        {
            // Step 1: Initialize core components (resources, scene, frame buffer, etc.)
            LOGGER_INFO("Initializing SimulationManager components...");
            init();

            // Step 2: Create viewer entities from the scene
            LOGGER_INFO("Creating simulation entities...");
            createEntities();

            // Step 3: Initialize graphics subsystem
            LOGGER_INFO("Initializing graphics subsystem...");
            viewer_->initGraphics();

            // Step 4: Initialize renderable entities in the viewer
            LOGGER_INFO("Initializing renderable entities...");
            viewer_->initEntities();

            // Step 5: Main simulation loop
            LOGGER_INFO("Starting main simulation loop...");

            // Performance monitoring variables
            static constexpr int PERFORMANCE_REPORT_INTERVAL = 300; // Report every 300 frames (~5 seconds at 60 FPS)
            int frameCounter = 0;

            while (!viewer_->shouldClose())
            {
                TIMER_SCOPE("SimulationLoop_Frame");

                float deltaTime = viewer_->getDeltaTime();

                // Update simulation state
                {
                    TIMER_SCOPE("SimulationLoop_Update");
                    update(deltaTime);
                    frameBuffer_->update(deltaTime);
                }

                // Update detected point cloud from signal solver
                if (signalSolver_ && detectedPointCloudEntity_)
                {
                    try
                    {
                        TIMER_SCOPE("SignalProcessing_Total");

                        // Time the critical signal solver operation
                        std::shared_ptr<math::PointCloud> pointCloud;
                        core::Timer::measure("SignalSolver_solve", [&]()
                                             { pointCloud = signalSolver_->solve(); });

                        // Time the point cloud update operation
                        core::Timer::measure("PointCloudEntity_setPointCloud", [&]()
                                             { detectedPointCloudEntity_->setPointCloud(std::move(pointCloud)); });
                    }
                    catch (const std::exception &e)
                    {
                        LOGGER_ERROR(std::string("Error in signal solver: ") + e.what());
                    }
                }

                // Render frame
                {
                    TIMER_SCOPE("SimulationLoop_Render");
                    render();
                }

                // // Periodic performance reporting
                // frameCounter++;
                // if (frameCounter >= PERFORMANCE_REPORT_INTERVAL)
                // {
                //     reportPerformanceStats();
                //     resetPerformanceStats();
                //     frameCounter = 0;
                // }
            }

            LOGGER_INFO("Simulation loop ended, cleaning up...");
            viewer_->cleanup();
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Fatal error in simulation: ") + e.what());
            if (viewer_)
            {
                viewer_->cleanup();
            }
            throw;
        }
    }

    void SimulationManager::onFrameChanged(const std::shared_ptr<simulation::Frame> &frame)
    {
        if (!frame)
        {
            LOGGER_ERROR("Received null frame in onFrameChanged");
            return;
        }

        try
        {
            // Update external point cloud entity
            if (!pcEntity_)
            {
                LOGGER_WARN("Point cloud entity is null, cannot update external point cloud");
                return;
            }
            pcEntity_->setPointCloud(frame->cloud);

            // Notify the scene about the new external point cloud
            if (scene_)
            {
                scene_->setExternalPointCloud(frame->cloud);
            }
            else
            {
                LOGGER_WARN("Scene is null, cannot update external point cloud in scene");
            }
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Error processing frame change: ") + e.what());
        }
    }

    void SimulationManager::reportPerformanceStats() const
    {
        auto frameStats = core::Timer::getTimerStats("SimulationLoop_Frame");

        // Only report if we have meaningful data
        if (frameStats.count < 10)
        {
            return; // Don't report with too few samples
        }

        LOGGER_INFO("=== PERFORMANCE WINDOW REPORT ===");
        LOGGER_INFO_F("Sample size: %lu frames", frameStats.count);

        // Detailed analysis of critical components
        auto signalSolverStats = core::Timer::getTimerStats("SignalSolver_solve");
        auto pointCloudStats = core::Timer::getTimerStats("PointCloudEntity_setPointCloud");
        auto signalProcessingStats = core::Timer::getTimerStats("SignalProcessing_Total");
        auto updateStats = core::Timer::getTimerStats("SimulationLoop_Update");
        auto renderStats = core::Timer::getTimerStats("SimulationLoop_Render");

        if (frameStats.count > 0)
        {
            double avgFPS = 1000.0 / frameStats.averageMs();
            LOGGER_INFO("=== FRAME PERFORMANCE ===");
            LOGGER_INFO_F("  - Average frame time: %.3f ms (%.1f FPS)", frameStats.averageMs(), avgFPS);
            LOGGER_INFO_F("  - Min frame time: %.3f ms", frameStats.minMs());
            LOGGER_INFO_F("  - Max frame time: %.3f ms", frameStats.maxMs());
        }

        if (signalSolverStats.count > 0)
        {
            LOGGER_INFO("=== SIGNAL SOLVER PERFORMANCE ===");
            LOGGER_INFO_F("  - Average solve time: %.3f ms", signalSolverStats.averageMs());
            LOGGER_INFO_F("  - Min solve time: %.3f ms", signalSolverStats.minMs());
            LOGGER_INFO_F("  - Max solve time: %.3f ms", signalSolverStats.maxMs());

            if (frameStats.count > 0)
            {
                double solverPercentage = (signalSolverStats.averageMs() / frameStats.averageMs()) * 100.0;
                LOGGER_INFO_F("  - Signal solver: %.1f%% of frame time", solverPercentage);
            }
        }

        if (updateStats.count > 0 && renderStats.count > 0)
        {
            LOGGER_INFO("=== BREAKDOWN BY COMPONENT ===");
            double updatePercentage = (updateStats.averageMs() / frameStats.averageMs()) * 100.0;
            double renderPercentage = (renderStats.averageMs() / frameStats.averageMs()) * 100.0;
            double signalPercentage = signalProcessingStats.count > 0 ? (signalProcessingStats.averageMs() / frameStats.averageMs()) * 100.0 : 0.0;

            LOGGER_INFO_F("  - Update: %.3f ms (%.1f%%)", updateStats.averageMs(), updatePercentage);
            LOGGER_INFO_F("  - Signal Processing: %.3f ms (%.1f%%)",
                          signalProcessingStats.averageMs(), signalPercentage);
            LOGGER_INFO_F("  - Render: %.3f ms (%.1f%%)", renderStats.averageMs(), renderPercentage);
        }

        LOGGER_INFO("=== END PERFORMANCE REPORT ===\n");
    }

    void SimulationManager::resetPerformanceStats()
    {
        TIMER_RESET();
        LOGGER_INFO("Performance statistics have been reset");
    }

} // namespace simulation
