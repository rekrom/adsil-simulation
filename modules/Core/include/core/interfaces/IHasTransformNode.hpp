#pragma once

#include <memory>
#include <core/TransformNode.hpp>

namespace core
{

    class IHasTransformNode
    {
    public:
        virtual ~IHasTransformNode() = default;

        virtual std::shared_ptr<TransformNode> getTransformNode() const = 0;
        virtual Transform getGlobalTransform() const = 0;
        virtual Transform &getLocalTransform() = 0;
    };
}
