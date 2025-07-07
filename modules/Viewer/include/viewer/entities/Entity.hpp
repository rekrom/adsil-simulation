#pragma once
#include <viewer/interfaces/IEntity.hpp>

namespace viewer
{
    class Entity : public IEntity
    {
    public:
        virtual ~Entity() = default;

        void setVisible(bool visible) override { visible_ = visible; }
        bool isVisible() const override { return visible_; }

        // Provide default fallback behavior
        bool isTransparent() const override { return false; }
        glm::vec3 getCenter() const override { return glm::vec3(0.0f); }

    protected:
        bool visible_ = true;
    };

}
