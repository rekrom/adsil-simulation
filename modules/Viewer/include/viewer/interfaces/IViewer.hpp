#pragma once

#include <viewer/entities/Entity.hpp>
#include <viewer/interfaces/IInputManager.hpp>
#include <viewer/common/RenderingTypes.hpp>
#include <core/Alias.hpp>
#include <memory>
#include <string>

namespace simulation
{
    class FrameBufferManager;
}

namespace viewer
{
    class PointCloudEntity;

    /**
     * @brief Abstract interface for viewer implementations.
     *
     * This interface allows decoupling of the simulation logic from
     * the concrete rendering implementation (e.g., OpenGL, Vulkan, or mock viewers for testing).
     */
    class IViewer
    {
    public:
        virtual ~IViewer() = default;

        // Lifecycle
        virtual void initGraphics() = 0;
        virtual void initEntities() = 0;
        virtual void render() = 0;
        virtual void cleanup() = 0;
        virtual bool shouldClose() const = 0;

        // Entity management
        virtual void addEntity(const std::shared_ptr<Entity> &entity) = 0;
        virtual void setEntities(SharedVec<Entity> entities) = 0;

        // Configuration
        virtual void setRenderingMode(RenderingMode mode) = 0;
        virtual RenderingMode getRenderingMode() const = 0;
        virtual void setFrameManager(std::shared_ptr<simulation::FrameBufferManager> frameBuffer) = 0;
        virtual void setSelectedPointCloudEntity(const std::shared_ptr<PointCloudEntity> &entity) = 0;

        // Accessors
        virtual float getDeltaTime() const = 0;
        virtual std::shared_ptr<input::IInputManager> getInputManager() const = 0;
    };

} // namespace viewer
