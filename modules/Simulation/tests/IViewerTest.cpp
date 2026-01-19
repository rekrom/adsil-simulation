// Test for IViewer interface and setViewer injection in SimulationManager

#include <simulation/implementations/SimulationManager.hpp>
#include <viewer/interfaces/IViewer.hpp>
#include <iostream>
#include <cassert>

namespace
{
    /**
     * @brief Mock implementation of IViewer for testing purposes.
     * 
     * This mock tracks method calls to verify that SimulationManager
     * correctly interacts with the viewer through the interface.
     */
    class MockViewer : public viewer::IViewer
    {
    public:
        // Call counters for verification
        int initGraphicsCalls = 0;
        int initEntitiesCalls = 0;
        int renderCalls = 0;
        int cleanupCalls = 0;
        int shouldCloseCalls = 0;
        int addEntityCalls = 0;
        int setEntitiesCalls = 0;
        int setRenderingModeCalls = 0;
        int setFrameManagerCalls = 0;
        int setSelectedPointCloudEntityCalls = 0;

        // Control for shouldClose behavior
        bool closeAfterRenders = true;
        int maxRenderCalls = 3;

        // IViewer interface implementation
        void initGraphics() override
        {
            ++initGraphicsCalls;
            std::cout << "[MockViewer] initGraphics called (" << initGraphicsCalls << ")" << std::endl;
        }

        void initEntities() override
        {
            ++initEntitiesCalls;
            std::cout << "[MockViewer] initEntities called (" << initEntitiesCalls << ")" << std::endl;
        }

        void render() override
        {
            ++renderCalls;
            std::cout << "[MockViewer] render called (" << renderCalls << ")" << std::endl;
        }

        void cleanup() override
        {
            ++cleanupCalls;
            std::cout << "[MockViewer] cleanup called (" << cleanupCalls << ")" << std::endl;
        }

        bool shouldClose() const override
        {
            ++const_cast<MockViewer*>(this)->shouldCloseCalls;
            bool result = closeAfterRenders && (renderCalls >= maxRenderCalls);
            std::cout << "[MockViewer] shouldClose called (" << shouldCloseCalls << ") -> " 
                      << (result ? "true" : "false") << std::endl;
            return result;
        }

        void addEntity(const std::shared_ptr<viewer::Entity> &entity) override
        {
            ++addEntityCalls;
            std::cout << "[MockViewer] addEntity called (" << addEntityCalls << ")" << std::endl;
        }

        void setEntities(SharedVec<viewer::Entity> entities) override
        {
            ++setEntitiesCalls;
            std::cout << "[MockViewer] setEntities called (" << setEntitiesCalls 
                      << ") with " << entities.size() << " entities" << std::endl;
        }

        void setRenderingMode(viewer::RenderingMode mode) override
        {
            ++setRenderingModeCalls;
            std::cout << "[MockViewer] setRenderingMode called (" << setRenderingModeCalls << ")" << std::endl;
            currentMode_ = mode;
        }

        viewer::RenderingMode getRenderingMode() const override
        {
            return currentMode_;
        }

        void setFrameManager(std::shared_ptr<simulation::FrameBufferManager> frameBuffer) override
        {
            ++setFrameManagerCalls;
            std::cout << "[MockViewer] setFrameManager called (" << setFrameManagerCalls << ")" << std::endl;
        }

        void setSelectedPointCloudEntity(const std::shared_ptr<viewer::PointCloudEntity> &entity) override
        {
            ++setSelectedPointCloudEntityCalls;
            std::cout << "[MockViewer] setSelectedPointCloudEntity called (" 
                      << setSelectedPointCloudEntityCalls << ")" << std::endl;
        }

        float getDeltaTime() const override
        {
            return 0.016f; // ~60 FPS
        }

        std::shared_ptr<viewer::input::IInputManager> getInputManager() const override
        {
            return nullptr; // No input manager for mock
        }

    private:
        viewer::RenderingMode currentMode_ = viewer::RenderingMode::Perspective;
    };

    void test_mockViewer_canBeCreated()
    {
        std::cout << "\n=== test_mockViewer_canBeCreated ===" << std::endl;
        
        auto mock = std::make_unique<MockViewer>();
        assert(mock != nullptr);
        
        std::cout << "[PASS] MockViewer can be created" << std::endl;
    }

    void test_mockViewer_implementsIViewer()
    {
        std::cout << "\n=== test_mockViewer_implementsIViewer ===" << std::endl;
        
        std::unique_ptr<viewer::IViewer> viewer = std::make_unique<MockViewer>();
        assert(viewer != nullptr);
        
        // Test interface methods
        viewer->initGraphics();
        viewer->initEntities();
        viewer->render();
        assert(!viewer->shouldClose()); // Not closed yet (only 1 render, need 3)
        viewer->render();
        viewer->render();
        assert(viewer->shouldClose()); // Now should close (3 renders)
        viewer->cleanup();
        
        std::cout << "[PASS] MockViewer implements IViewer interface correctly" << std::endl;
    }

    void test_setViewer_acceptsMockViewer()
    {
        std::cout << "\n=== test_setViewer_acceptsMockViewer ===" << std::endl;
        
        auto manager = std::make_shared<simulation::SimulationManager>();
        auto mock = std::make_unique<MockViewer>();
        
        // This should compile and work - tests the setViewer method
        manager->setViewer(std::move(mock));
        
        std::cout << "[PASS] SimulationManager::setViewer accepts MockViewer" << std::endl;
    }

    void test_interface_polymorphism()
    {
        std::cout << "\n=== test_interface_polymorphism ===" << std::endl;
        
        // Create viewer through interface pointer
        std::unique_ptr<viewer::IViewer> viewer = std::make_unique<MockViewer>();
        
        // Downcast to check call counts
        MockViewer* mockPtr = dynamic_cast<MockViewer*>(viewer.get());
        assert(mockPtr != nullptr);
        
        // Call through interface
        viewer->initGraphics();
        viewer->setEntities({});
        viewer->render();
        
        // Verify calls went through
        assert(mockPtr->initGraphicsCalls == 1);
        assert(mockPtr->setEntitiesCalls == 1);
        assert(mockPtr->renderCalls == 1);
        
        std::cout << "[PASS] Interface polymorphism works correctly" << std::endl;
    }

} // anonymous namespace

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "IViewer Interface Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    test_mockViewer_canBeCreated();
    test_mockViewer_implementsIViewer();
    test_setViewer_acceptsMockViewer();
    test_interface_polymorphism();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "All IViewer tests passed!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
