#pragma once
#include <viewer/interfaces/IEntity.hpp>
#include <viewer/renderables/Renderable.hpp>
#include <viewer/renderables/renderables.hpp>
#include <core/Logger.hpp>
namespace viewer
{
    class Entity : public IEntity
    {
    public:
        virtual ~Entity() = default;

        void setName(const std::string &name) override
        {
            name_ = name;
        }
        std::string getName() const override
        {
            if (name_ == "Entity")
            {
                LOGGER_WARN_F("Entity::getName: Default name '%s' used, consider setting a specific name for better identification.", name_.c_str());
            }
            return name_;
        }

        void setVisible(bool visible) override { visible_ = visible; }
        bool isVisible() const override { return visible_; }

        virtual void initGL() override
        {
            if (!renderable_)
            {
                LOGGER_ERROR_F("Entity::initGL: renderable not found for %s", name_.c_str());
                throw std::runtime_error("Entity::initGL: renderable not found for " + name_);
            }

            renderable_->initGL();
        }

        virtual void render(const glm::mat4 &view, const glm::mat4 &projection) override
        {
            if (!renderable_)
            {
                throw std::runtime_error("Entity::render: renderable not found for " + getName());
            }
            if (!isVisible())
            {
                LOGGER_DEBUG_F("Entity::render: %s is not visible, skipping render.", getName().c_str());
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
                LOGGER_ERROR_F("Entity::cleanup: renderable not found for %s", name_.c_str());
                return;
            }
            renderable_->cleanup();
        }

        virtual glm::vec3 getCenter() const override
        {
            if (!renderable_)
            {
                LOGGER_ERROR_F("Entity::getCenter: renderable not found for %s", name_.c_str());
                return glm::vec3(0.0f); // Default center if no renderable
            }
            return renderable_->getCenter();
        }

        virtual bool isTransparent() const override
        {
            if (!renderable_)
            {
                LOGGER_ERROR_F("Entity::isTransparent: renderable not found for %s", name_.c_str());
                return false;
            }

            if (renderable_->isTransparent())
                return true;

            for (const auto &sub : renderable_->getSubRenderables())
            {
                if (sub && sub->isTransparent())
                    return true;
            }

            return false;
        }

        // 🆕 Unified renderable ownership
        void setRenderable(std::shared_ptr<Renderable> renderable)
        {
            renderable_ = std::move(renderable);
        }

        std::shared_ptr<Renderable> getRenderable() const
        {
            return renderable_;
        }

    protected:
        std::string name_{"Entity"};
        std::shared_ptr<Renderable> renderable_; // 🆙 moved here
        bool visible_ = true;
    };
}
