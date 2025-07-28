#pragma once
#include <viewer/interfaces/IEntity.hpp>
#include <viewer/renderables/Renderable.hpp>
#include <core/Logger.hpp>
namespace viewer
{
    class Entity : public IEntity
    {
    public:
        virtual ~Entity() = default;

        void setName(const std::string &name) override { name_ = name; }
        std::string getName() const override
        {
            if (name_ == "Entity")
            {
                LOGGER_WARN("Entity::getName: Default name 'Entity' used, consider setting a specific name for better identification.");
            }
            return name_;
        }

        void setVisible(bool visible) override { visible_ = visible; }
        bool isVisible() const override { return visible_; }

        // 🆕 Unified renderable ownership
        void setRenderable(std::shared_ptr<Renderable> renderable) { renderable_ = std::move(renderable); }
        std::shared_ptr<Renderable> getRenderable() const { return renderable_; }

        virtual void initGL() override
        {
            if (!renderable_)
            {
                LOGGER_ERROR("Entity::initGL: renderable not found for " + getName());
                throw std::runtime_error("Entity::initGL: renderable not found for " + getName());
            }

            renderable_->initGL();
        }

        virtual void render(const glm::mat4 &view, const glm::mat4 &projection) override
        {
            if (!isVisible() || !renderable_)
            {
                LOGGER_ERROR("Entity::render: renderable not found for " + getName());
                // Optionally throw an exception or log a warning
                return;
            }
            renderable_->render(view, projection);

            for (const auto &subRenderable : renderable_->getSubRenderables())
            {
                if (!subRenderable)
                {
                    return;
                }
                subRenderable->render(view, projection);
            }
        }

        virtual void cleanup() override
        {
            if (!renderable_)
            {
                LOGGER_ERROR("Entity::cleanup: renderable not found for " + getName());
                return;
            }
            renderable_->cleanup();
        }

        virtual glm::vec3 getCenter() const override
        {
            if (!renderable_)
            {
                LOGGER_ERROR("Entity::getCenter: renderable not found for " + getName());
                return glm::vec3(0.0f); // Default center if no renderable
            }
            return renderable_->getCenter();
        }

        virtual bool isTransparent() const override
        {
            if (!renderable_)
            {
                LOGGER_ERROR("Entity::isTransparent: renderable not found for " + getName());
                return false; // Default to non-transparent if no renderable
            }
            return renderable_->isTransparent();
        }

    protected:
        std::string name_{"Entity"};
        std::shared_ptr<Renderable> renderable_; // 🆙 moved here
        bool visible_ = true;
    };
}
