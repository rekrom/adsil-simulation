// HasTransform.hpp
#pragma once

#include <spatial/interfaces/interfaces.hpp>
#include <spatial/implementations/TransformNode.hpp>

namespace spatial
{
    class HasTransform : public ITransform
    {
    public:
        virtual ~HasTransform() = default;

        std::shared_ptr<TransformNode> getTransformNode() const override
        {
            return transformNode_;
        }

        void setTransformNode(std::shared_ptr<TransformNode> node)
        {
            transformNode_ = std::move(node);
        }

        Transform getGlobalTransform() const override
        {
            assert(transformNode_ && "transformNode_ is null in getGlobalTransform()");

            return transformNode_->getGlobalTransform();
        }

        Transform &getLocalTransform() override
        {
            static Transform dummy;
            return dummy;
            // return transformNode_->getLocalTransform();
        }

    protected:
        std::shared_ptr<TransformNode> transformNode_;
    };
}
