#include <simulation/implementations/SimulationManager.hpp>
#include <viewer/entities/GroundEntity.hpp>
#include <viewer/entities/AxisEntity.hpp>
#include <viewer/entities/PointCloudEntity.hpp>

#ifndef BASE_RESOURCE_DIR
#define BASE_RESOURCE_DIR "/home/rkrm-dev/Desktop/adsil_analyzer_cpp/resources"
#endif

namespace simulation
{
    void SimulationManager::init()
    {
        // core::Logger::getInstance().enableSyslog(true);

        // Set the base path for resolving resources
        core::ResourceLocator::setBasePath(BASE_RESOURCE_DIR);

        // Initialize adapter system
        adapters_ = std::make_unique<adapter::AdapterManager>();

        // Load the simulation scene (includes car, devices, etc.)
        scene_ = adapters_->fromJson<std::shared_ptr<SimulationScene>>(
            core::ResourceLocator::getJsonPath("scene.json"));

        // Fail fast if essential components are missing
        if (!scene_ || !scene_->getCar())
            throw std::runtime_error("Missing essential simulation components (scene or car).");

        // Initialize frame buffer with a ±3 frame window (total = 7)
        frameBuffer_ = std::make_shared<FrameBufferManager>(3);

        // Initialize the OpenGL viewer
        viewer_ = std::make_unique<viewer::OpenGLViewer>(1280, 720, "ADSIL Analyzer - OpenGL");
        viewer_->setFrameManager(frameBuffer_);

        // Initialize the input manager (for controlling the simulation)
        inputManager_ = std::make_shared<simulation::InputManager>();

        // Initialize the signal solver (sensor signal processing, etc.)
        signalSolver_ = std::make_shared<SignalSolver>(scene_);

        //
        // Register the signal solver as a frame observer
        frameBuffer_->addFrameObserver(signalSolver_);
        // Register the scene as a frame observer
        frameBuffer_->addFrameObserver(scene_);
    }

    void SimulationManager::createEntities()
    {
        SharedVec<viewer::Entity> entities;

        auto axisEntity = std::make_shared<viewer::AxisEntity>();
        auto groundEntity = std::make_shared<viewer::GroundEntity>();
        auto carEntity = std::make_shared<viewer::CarEntity>(scene_->getCar(), glm::vec3(0.2F, 0.6F, 0.9F));
        auto rxDevices = carEntity->getRxEntities();
        auto txDevices = carEntity->getTxEntities();

        entities.push_back(axisEntity);
        entities.push_back(groundEntity);
        entities.push_back(carEntity);
        for (const auto &tx : txDevices)
        {
            entities.push_back(tx);
        }
        for (const auto &rx : rxDevices)
        {
            entities.push_back(rx);
        }

        for (auto shape : scene_->getShapes())
        {
            auto shapeEntity = std::make_shared<viewer::ShapeEntity>(shape);
            entities.push_back(shapeEntity);
        }

        auto pcEntity = std::make_shared<viewer::PointCloudEntity>(this->scene_->getExternalPointCloud());
        pcEntity->setName("Outside Point Cloud");
        // In createEntities():
        pcEntityObserver_ = std::make_shared<viewer::PointCloudEntityObserver>(pcEntity);
        frameBuffer_->addFrameObserver(pcEntityObserver_);

        entities.push_back(pcEntity);

        detectedPointCloudEntity_ = std::make_shared<viewer::PointCloudEntity>();
        detectedPointCloudEntity_->setName("Detected Point Cloud");
        viewer_->setSelectedPointCloudEntity(detectedPointCloudEntity_);

        detectedPointCloudEntity_->setPointSize(2.0F);
        detectedPointCloudEntity_->setAlpha(1.0F);
        detectedPointCloudEntity_->setColor(glm::vec3(110.0F / 255.0F, 1.0F, 160.0F / 255.0F));

        entities.push_back(detectedPointCloudEntity_);

        viewer_->setEntities(entities);
    }

    void SimulationManager::update(float deltaTime)
    {
        inputManager_->processInput(deltaTime, *scene_->getCar(), viewer_->getCamera(), frameBuffer_);
    }

    void SimulationManager::render()
    {
        viewer_->render();
    }

    void SimulationManager::run()
    {
        // Step 1: Initialize core components (resources, scene, frame buffer, etc.)
        init();

        // Step 2: Create viewer entities from the scene
        createEntities();

        // Step 3: Initialize graphics subsystem
        try
        {
            viewer_->initGraphics();
        }
        catch (const std::exception &e)
        {
            LOGGER_ERROR(std::string("Initialization error: ") + e.what());
            return;
        }

        // Step 4: Initialize renderable entities in the viewer
        viewer_->initEntities();

        // Step 5: Main simulation loop
        while (!viewer_->shouldClose())
        {
            float deltaTime = viewer_->getDeltaTime();
            update(deltaTime);
            frameBuffer_->update(deltaTime);

            detectedPointCloudEntity_->addPoints(signalSolver_->solve()->getPoints());
            render();
        }
        viewer_->cleanup();
    }

} // namespace simulation
