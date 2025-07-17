#pragma once

#include <memory>
#include <spatial/implementations/TransformNode.hpp>

namespace spatial
{
    class ITransform
    {
    public:
        virtual ~ITransform() = default;

        virtual std::shared_ptr<TransformNode> getTransformNode() const = 0;
        virtual Transform getGlobalTransform() const = 0;
    };
}
