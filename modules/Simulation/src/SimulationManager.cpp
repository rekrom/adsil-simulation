#include <simulation/implementations/SimulationManager.hpp>
#include <viewer/entities/GroundEntity.hpp>
#include <viewer/entities/AxisEntity.hpp>
#include <viewer/entities/PointCloudEntity.hpp>
#include <core/Timer.hpp>
#include <utils/DataExporter.hpp>
#include <stdexcept>

// Fallback base resource directory if environment variable is not set
#ifndef ADSIL_RESOURCE_PATH_DEFAULT
#define ADSIL_RESOURCE_PATH_DEFAULT ROOT_PATH_DEFAULT
#endif

namespace simulation
{
    namespace // internal linkage for constants
    {
        constexpr const char *LogChannel = "SimulationManager";
        constexpr int kPerformanceReportIntervalFrames = 300; // ~5s at 60 FPS
        constexpr int kTimestampLogSample = 120;              // log every ~2s when playing
    }

    SimulationManager::SimulationManager()
        : config_(SimulationConfig::createDefault())
    {
        // print out the base path
        LOGGER_INFO(LogChannel, "Base path: " + config_->getResourceConfig().basePath);
    }

    SimulationManager::SimulationManager(std::shared_ptr<SimulationConfig> config)
        : config_(config)
    {
        observerName_ = "SimulationManagerObserver";

        if (!config_)
        {
            config_ = SimulationConfig::createDefault();
            LOGGER_INFO(LogChannel, "No configuration provided, using default.");
        }
    }

    void SimulationManager::init()
    {
        // Set observer name for debugging
        observerName_ = "SimulationManagerObserver";
        // Setup simulation logger with file output

        try
        {
            initializeComponents();
            validateEssentialComponents();
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("SimulationManager initialization failed: ") + e.what());
            throw;
        }
    }

    void SimulationManager::initializeComponents()
    {
        const auto &frameConfig = config_->getFrameConfig();

        // Initialize adapter system
        adapters_ = std::make_unique<adapter::AdapterManager>();

        // Load the simulation scene (includes car, devices, etc.)
        scene_ = adapters_->fromJson<std::shared_ptr<SimulationScene>>(
            core::ResourceLocator::getJsonPath("scene.json"));

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
        signalSolver_ = std::make_shared<simulation::SignalSolver>(scene_);

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
            LOGGER_ERROR(LogChannel, "Cannot create entities: scene is null");
            return;
        }

        const auto &carConfig = config_->getCarConfig();
        const auto &pointCloudConfig = config_->getPointCloudConfig();

        SharedVec<viewer::Entity> entities;

        // Create basic scene entities
        auto axisEntity = std::make_shared<viewer::AxisEntity>();
        auto groundEntity = std::make_shared<viewer::GroundEntity>();
        auto carEntity = std::make_shared<viewer::CarEntity>(scene_->getCar(), carConfig.color);

        entities.push_back(axisEntity);
        entities.push_back(groundEntity);
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
            LOGGER_ERROR(LogChannel, "Cannot update: essential components are null");
            return;
        }

        try
        {
            inputManager_->processInput(deltaTime, *scene_->getCar(), frameBuffer_);
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error during update: ") + e.what());
        }
    }

    void SimulationManager::render()
    {
        if (!viewer_)
        {
            LOGGER_ERROR(LogChannel, "Cannot render: viewer is null");
            return;
        }

        try
        {
            viewer_->render();
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error during rendering: ") + e.what());
        }
    }

    void SimulationManager::play()
    {
        if (!frameBuffer_)
        {
            LOGGER_ERROR(LogChannel, "Cannot play: frame buffer is null");
            return;
        }

        try
        {
            frameBuffer_->play();
            LOGGER_INFO(LogChannel, "Simulation playback started");
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error starting playback: ") + e.what());
        }
    }

    void SimulationManager::pause()
    {
        if (!frameBuffer_)
        {
            LOGGER_ERROR(LogChannel, "Cannot pause: frame buffer is null");
            return;
        }

        try
        {
            frameBuffer_->pause();
            LOGGER_INFO(LogChannel, "Simulation playback paused");
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error pausing playback: ") + e.what());
        }
    }

    void SimulationManager::seek(int frameIndex)
    {
        if (!frameBuffer_)
        {
            LOGGER_ERROR(LogChannel, "Cannot seek: frame buffer is null");
            return;
        }

        try
        {
            frameBuffer_->seek(frameIndex);
            LOGGER_INFO(LogChannel, std::string("Seeking to frame ") + std::to_string(frameIndex));
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error seeking to frame ") + std::to_string(frameIndex) + ": " + e.what());
        }
    }

    void SimulationManager::processSignals_()
    {
        if (!signalSolver_ || !detectedPointCloudEntity_)
            return;

        if (!frameBuffer_ || !hasFrameChanged_)
            return; // silent fast path (avoid log spam)

        hasFrameChanged_ = false;
        // Single concise INFO log before solve for traceability (timestamp + frame)
        const auto ts = frameBuffer_->getCurrentTimestamp();
        const auto frameIdx = frameBuffer_->getCurrentFrameIndex();
        const auto totalFrames = frameBuffer_->getTotalFrameCount();
        LOGGER_INFO("simulation", std::string("solve_start ts=") + std::to_string(ts) +
                                      " frame=" + std::to_string(frameIdx) + "/" + std::to_string(totalFrames));

        // Set frame context for data export
        utils::DataExporter::getInstance().setFrameContext(frameIdx, ts);

        try
        {
            TIMER_SCOPE("SignalProcessing_Total");

            std::shared_ptr<math::PointCloud> pointCloud;
            core::Timer::measure("SignalSolver_solve", [&]()
                                 { pointCloud = signalSolver_->solve(); });
            core::Timer::measure("PointCloudEntity_setPointCloud", [&]()
                                 { detectedPointCloudEntity_->setPointCloud(pointCloud); });
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error in signal solver: ") + e.what());
        }
    }

    void SimulationManager::run()
    {
        try
        {
            // Set the base path for resolving resources
            core::ResourceLocator::setBasePath(config_->getResourceConfig().basePath);
            auto &simLogger = core::Logger::getInstance(LogChannel);
            simLogger.setLogFile(core::ResourceLocator::getLoggingPath("simulation_manager.log"));
            // clear the current simulation log
            simLogger.clearLog();
            // simLogger.enableSyslog();

            const char *simulationOutput = "simulation";
            auto &simOutputLogger = core::Logger::getInstance(simulationOutput);
            simOutputLogger.setLogFile(core::ResourceLocator::getLoggingPath("simulation.log"));
            simOutputLogger.clearLog();

            // Initialize data exporter for CSV output
            auto &exporter = utils::DataExporter::getInstance();
            exporter.init(core::ResourceLocator::getExportPath());
            exporter.startSession();
            LOGGER_INFO(LogChannel, "Data exporter initialized: " + exporter.getCurrentFilePath());

            // Step 1: Initialize core components (resources, scene, frame buffer, etc.)
            LOGGER_INFO(LogChannel, "Initializing SimulationManager components...");
            init();

            // Step 2: Create viewer entities from the scene
            LOGGER_INFO(LogChannel, "Creating simulation entities...");
            createEntities();

            // Step 3: Initialize graphics subsystem
            LOGGER_INFO(LogChannel, "Initializing graphics subsystem...");
            viewer_->initGraphics();

            // Step 4: Initialize renderable entities in the viewer
            LOGGER_INFO(LogChannel, "Initializing renderable entities...");
            viewer_->initEntities();

            // Step 5: Main simulation loop
            LOGGER_INFO(LogChannel, "Starting main simulation loop...");

            // Performance monitoring variables
            int perfFrameCounter = 0; // counts frames for performance reporting

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

                // Signal processing (sampled logging inside)
                processSignals_();

                // Render frame
                {
                    TIMER_SCOPE("SimulationLoop_Render");
                    render();
                }

                // Periodic performance reporting (enabled, configurable via constant for now)
                if (++perfFrameCounter >= kPerformanceReportIntervalFrames)
                {
                    reportPerformanceStats();
                    resetPerformanceStats();
                    perfFrameCounter = 0;
                }
            }

            LOGGER_INFO(LogChannel, "Simulation loop ended, cleaning up...");
            utils::DataExporter::getInstance().endSession();
            viewer_->cleanup();
        }
        catch (const std::exception &e)
        {
            if (viewer_)
            {
                viewer_->cleanup();
            }
            throw std::runtime_error("SimulationManager error: " + std::string(e.what()));
        }
    }

    void SimulationManager::onFrameChanged(const std::shared_ptr<simulation::Frame> &frame)
    {
        if (!frame)
        {
            LOGGER_ERROR(LogChannel, "Received null frame in onFrameChanged");
            return;
        }

        try
        {
            // Update external point cloud entity
            if (!pcEntity_)
            {
                LOGGER_WARN(LogChannel, "Point cloud entity is null, cannot update external point cloud");
                return;
            }
            hasFrameChanged_ = true;
            pcEntity_->setPointCloud(frame->cloud);

            // Notify the scene about the new external point cloud
            if (scene_)
            {
                scene_->setExternalPointCloud(frame->cloud);
            }
            else
            {
                LOGGER_WARN(LogChannel, "Scene is null, cannot update external point cloud in scene");
            }
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(LogChannel, std::string("Error processing frame change: ") + e.what());
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

        LOGGER_INFO(LogChannel, "=== PERFORMANCE WINDOW REPORT ===");
        LOGGER_INFO(LogChannel, std::string("Sample size: ") + std::to_string(frameStats.count) + " frames");

        // Detailed analysis of critical components
        auto signalSolverStats = core::Timer::getTimerStats("SignalSolver_solve");
        auto pointCloudStats = core::Timer::getTimerStats("PointCloudEntity_setPointCloud");
        auto signalProcessingStats = core::Timer::getTimerStats("SignalProcessing_Total");
        auto updateStats = core::Timer::getTimerStats("SimulationLoop_Update");
        auto renderStats = core::Timer::getTimerStats("SimulationLoop_Render");

        if (frameStats.count > 0)
        {
            double avgFPS = 1000.0 / frameStats.averageMs();
            LOGGER_INFO(LogChannel, "=== FRAME PERFORMANCE ===");
            LOGGER_INFO(LogChannel, "  - Average frame time: " + std::to_string(frameStats.averageMs()) + " ms (" + std::to_string(avgFPS) + " FPS)");
            LOGGER_INFO(LogChannel, "  - Min frame time: " + std::to_string(frameStats.minMs()) + " ms");
            LOGGER_INFO(LogChannel, "  - Max frame time: " + std::to_string(frameStats.maxMs()) + " ms");
        }

        if (signalSolverStats.count > 0)
        {
            LOGGER_INFO(LogChannel, "=== SIGNAL SOLVER PERFORMANCE ===");
            LOGGER_INFO(LogChannel, "  - Average solve time: " + std::to_string(signalSolverStats.averageMs()) + " ms");
            LOGGER_INFO(LogChannel, "  - Min solve time: " + std::to_string(signalSolverStats.minMs()) + " ms");
            LOGGER_INFO(LogChannel, "  - Max solve time: " + std::to_string(signalSolverStats.maxMs()) + " ms");

            if (frameStats.count > 0)
            {
                double solverPercentage = (signalSolverStats.averageMs() / frameStats.averageMs()) * 100.0;
                LOGGER_INFO(LogChannel, "  - Signal solver: " + std::to_string(solverPercentage) + "% of frame time");
            }
        }

        if (updateStats.count > 0 && renderStats.count > 0)
        {
            LOGGER_INFO(LogChannel, "=== BREAKDOWN BY COMPONENT ===");
            double updatePercentage = (updateStats.averageMs() / frameStats.averageMs()) * 100.0;
            double renderPercentage = (renderStats.averageMs() / frameStats.averageMs()) * 100.0;
            double signalPercentage = signalProcessingStats.count > 0 ? (signalProcessingStats.averageMs() / frameStats.averageMs()) * 100.0 : 0.0;

            LOGGER_INFO(LogChannel, "  - Update: " + std::to_string(updateStats.averageMs()) + " ms (" + std::to_string(updatePercentage) + "%)");
            LOGGER_INFO(LogChannel, "  - Signal Processing: " + std::to_string(signalProcessingStats.averageMs()) + " ms (" + std::to_string(signalPercentage) + "%)");
            LOGGER_INFO(LogChannel, "  - Render: " + std::to_string(renderStats.averageMs()) + " ms (" + std::to_string(renderPercentage) + "%)");
        }

        LOGGER_INFO(LogChannel, "=== END PERFORMANCE REPORT ===\n");
    }

    void SimulationManager::resetPerformanceStats()
    {
        TIMER_RESET();
        // LOGGER_INFO(LogChannel, "Performance statistics have been reset");
    }

} // namespace simulation
