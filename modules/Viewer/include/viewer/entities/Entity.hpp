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

        void setVisible(bool visible) override { visible_ = visible; }
        bool isVisible() const override { return visible_; }

        // 🆕 Unified renderable ownership
        void setRenderable(std::shared_ptr<Renderable> renderable) { renderable_ = std::move(renderable); }
        std::shared_ptr<Renderable> getRenderable() const { return renderable_; }

        virtual void initGL() override
        {
            if (!renderable_)
            {
                std::string name = getName();
                LOGGER_ERROR("Entity::initGL: renderable not found for " + name);
                throw std::runtime_error("Entity::initGL: renderable not found for " + name);
            }

            renderable_->initGL();
        }

    protected:
        std::shared_ptr<Renderable> renderable_; // 🆙 moved here
        bool visible_ = true;
    };
}
