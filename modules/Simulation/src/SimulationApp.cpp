#include <simulation/implementations/SimulationApp.hpp>
#include <viewer/entities/GroundEntity.hpp>
#include <viewer/entities/AxisEntity.hpp>

#ifndef BASE_RESOURCE_DIR
#define BASE_RESOURCE_DIR "./resources"
#endif

namespace simulation
{
    void SimulationApp::init()
    {
        core::ResourceLocator::setBasePath(BASE_RESOURCE_DIR);

        adapters = std::make_unique<adapter::AdapterManager>();

        auto ground = std::make_shared<spatial::TransformNode>();

        car_ = adapters->fromJson<std::shared_ptr<Car>>(core::ResourceLocator::getJsonPath("car.json"));

        // Attach car's transform node as a child of ground
        ground->addChild(car_->getTransformNode());

        // Calculate height offset
        float groundHeight = ground->getGlobalTransform().getPosition().y();
        float carHeight = car_->getDimension().height;

        // Set car's local position on top of ground
        spatial::Transform carTransform = car_->getTransformNode()->getLocalTransform();
        carTransform.setPosition(Point(0.0F, groundHeight / 2.0F + carHeight / 2.0F, 0.0F));
        car_->getTransformNode()->setLocalTransform(carTransform);

        scene_ = adapters->fromJson<std::shared_ptr<SimulationScene>>(core::ResourceLocator::getJsonPath("objects.json"));

        if (!scene_ || !car_)
            throw std::runtime_error("Missing essential simulation components.");

        std::cout << "[INFO] Car loaded." << std::endl;
        std::cout << "[INFO] Scene loaded." << std::endl;
        viewer_ = std::make_unique<viewer::OpenGLViewer>(1280, 720, "ADSIL Analyzer - OpenGL");

        inputManager_ = std::make_shared<simulation::InputManager>();
    }

    void SimulationApp::createEntities()
    {
        SharedVec<viewer::Entity> entities;

        auto axisEntity = std::make_shared<viewer::AxisEntity>();
        auto groundEntity = std::make_shared<viewer::GroundEntity>();
        auto carEntity = std::make_shared<viewer::CarEntity>(car_, glm::vec3(0.2F, 0.6F, 0.9F));

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

    void SimulationApp::update(float deltaTime)
    {
        inputManager_->processInput(deltaTime, *car_, viewer_->getCamera());
    }

    void SimulationApp::render()
    {
        viewer_->render();
    }

    void SimulationApp::run()
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
            render();
            // car_->moveForward(0.01F);
        }
        viewer_->cleanup();
    }

} // namespace simulation
