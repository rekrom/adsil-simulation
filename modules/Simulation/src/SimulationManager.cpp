#include <simulation/implementations/SimulationManager.hpp>
#include <viewer/entities/GroundEntity.hpp>
#include <viewer/entities/AxisEntity.hpp>

#ifndef BASE_RESOURCE_DIR
#define BASE_RESOURCE_DIR "./resources"
#endif

namespace simulation
{
    void SimulationManager::init()
    {
        core::ResourceLocator::setBasePath(BASE_RESOURCE_DIR);

        adapters_ = std::make_unique<adapter::AdapterManager>();

        scene_ = adapters_->fromJson<std::shared_ptr<SimulationScene>>(core::ResourceLocator::getJsonPath("scene.json"));

        if (!scene_ || !scene_->getCar())
            throw std::runtime_error("Missing essential simulation components.");

        viewer_ = std::make_unique<viewer::OpenGLViewer>(1280, 720, "ADSIL Analyzer - OpenGL");

        inputManager_ = std::make_shared<simulation::InputManager>();
        signalSolver_ = std::make_unique<SignalSolver>(scene_);
    }

    void SimulationManager::createEntities()
    {
        SharedVec<viewer::Entity> entities;

        auto axisEntity = std::make_shared<viewer::AxisEntity>();
        auto groundEntity = std::make_shared<viewer::GroundEntity>();
        auto carEntity = std::make_shared<viewer::CarEntity>(scene_->getCar(), glm::vec3(0.2F, 0.6F, 0.9F));

        entities.push_back(groundEntity);
        entities.push_back(axisEntity);
        entities.push_back(carEntity);

        for (auto shape : scene_->getShapes())
        {
            auto shapeEntity = std::make_shared<viewer::ShapeEntity>(shape);

            entities.push_back(shapeEntity);
        }

        viewer_->setEntities(entities);
    }

    void SimulationManager::update(float deltaTime)
    {
        inputManager_->processInput(deltaTime, *scene_->getCar(), viewer_->getCamera());
    }

    void SimulationManager::render()
    {
        viewer_->render();
    }

    void SimulationManager::run()
    {
        init();
        createEntities();

        try
        {
            viewer_->initGraphics();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            return;
        }
        viewer_->initEntities();

        while (!viewer_->shouldClose())
        {
            float deltaTime = viewer_->getDeltaTime();
            update(deltaTime);
            // signalSolver_->solve();
            render();
            // car_->moveForward(0.01F);
        }
        viewer_->cleanup();
    }

} // namespace simulation
