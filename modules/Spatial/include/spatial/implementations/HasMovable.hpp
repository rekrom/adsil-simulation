#pragma once

#include <spatial/interfaces/IMovable.hpp>
#include <spatial/implementations/HasTransform.hpp>
#include <iostream>

namespace spatial
{
    class HasMovable : public spatial::HasTransform, public IMovable
    {
    public:
        virtual ~HasMovable() = default;

        void moveForward(float delta) override
        {
            if (auto node = getTransformNode())
            {
                Vector direction = node->getGlobalTransform().get3DDirectionVector();
                Transform local = node->getLocalTransform();
                local.move(direction * (delta * speed_));
                node->setLocalTransform(local);
            }
            else
            {
                throw std::runtime_error("Transform node NOT found");
            }
        }

        void rotateYaw(float delta) override
        {
            if (auto node = getTransformNode())
            {
                Transform local = node->getLocalTransform();
                local.rotateYaw(delta * speed_);
                node->setLocalTransform(local);
            }
        }

        void setSpeed(float speed) override { speed_ = speed; }
        [[nodiscard]] float getSpeed() const override { return speed_; }

    protected:
        float speed_ = 1.0f; // default speed
    };
}