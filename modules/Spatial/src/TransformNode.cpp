#include <spatial/implementations/TransformNode.hpp>
#include <algorithm> // for std::remove
#include <iostream>
namespace spatial
{

    // Constructor: default local transform (identity)
    TransformNode::TransformNode()
        : localTransform_(Transform()), dirty_(true) {}

    // Constructor: initialize with given local transform
    TransformNode::TransformNode(const Transform &localTransform)
        : localTransform_(localTransform), dirty_(true)
    {
        // std::cout << localTransform_.get3DDirectionVector().toString() << std::endl;
    }

    void TransformNode::setLocalTransform(const Transform &transform)
    {
        localTransform_ = transform;
        dirty_ = true;
        // Mark all children dirty too
        for (auto &child : children_)
        {
            child->dirty_ = true;
        }
    }

    const Transform &TransformNode::getLocalTransform() const
    {
        return localTransform_;
    }

    Transform TransformNode::getGlobalTransform() const
    {
        if (dirty_)
        {
            updateGlobalTransform();
        }
        return cachedGlobalTransform_;
    }

    void TransformNode::setParent(std::shared_ptr<TransformNode> parent)
    {
        // Remove from old parent's children if exists
        if (auto oldParent = parent_.lock())
        {
            auto &siblings = oldParent->children_;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), shared_from_this()), siblings.end());
        }

        parent_ = parent;

        if (parent)
        {
            parent->children_.push_back(shared_from_this());
        }

        dirty_ = true;
    }

    std::shared_ptr<TransformNode> TransformNode::getParent() const
    {
        return parent_.lock();
    }

    void TransformNode::addChild(std::shared_ptr<TransformNode> child)
    {
        if (child)
        {
            child->setParent(shared_from_this());
        }
    }

    void TransformNode::removeChild(std::shared_ptr<TransformNode> child)
    {
        if (child && !children_.empty())
        {
            auto it = std::remove(children_.begin(), children_.end(), child);
            if (it != children_.end())
            {
                children_.erase(it, children_.end());
                child->parent_.reset();
                child->dirty_ = true;
            }
        }
    }

    const std::vector<std::shared_ptr<TransformNode>> &TransformNode::getChildren() const
    {
        return children_;
    }

    void TransformNode::updateGlobalTransform() const
    {
        if (auto parent = parent_.lock())
        {
            cachedGlobalTransform_ = parent->getGlobalTransform() * localTransform_;
        }
        else
        {
            cachedGlobalTransform_ = localTransform_;
        }
        dirty_ = false;

        // Propagate dirty flag to children
        for (auto &child : children_)
        {
            child->dirty_ = true;
        }
    }

} // namespace spatial
