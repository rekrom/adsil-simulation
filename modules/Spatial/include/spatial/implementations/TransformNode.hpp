#pragma once

#include <core/Alias.hpp>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Transform.hpp" // assuming you have this for position/orientation storage

namespace spatial
{

    /**
     * @class TransformNode
     * @brief Hierarchical transform node for scene graph.
     *
     * @note Thread Safety: This class is NOT thread-safe. All access should be from
     *       a single thread. The mutable cache members are for lazy evaluation
     *       optimization in const methods (getGlobalTransform).
     */
    class TransformNode : public std::enable_shared_from_this<TransformNode>
    {
    public:
        TransformNode() = default;

        explicit TransformNode(Transform localTransform);

        // Setters and getters for local transform
        void setLocalTransform(const Transform &transform);
        const Transform &getLocalTransform() const;

        // Get the global/world transform (computed by combining with parent)
        Transform getGlobalTransform() const;

        // Parent management
        void setParent(const std::shared_ptr<TransformNode> &parent);
        std::shared_ptr<TransformNode> getParent() const;

        // Children management

        void addChild(const std::shared_ptr<TransformNode> &child);
        void removeChild(const std::shared_ptr<TransformNode> &child);
        const SharedVec<TransformNode> &getChildren() const;

    private:
        // Cached global transform and dirty flag for lazy updates.
        // Note: mutable for lazy evaluation in const methods - not thread-safe
        mutable Transform cachedGlobalTransform_;
        mutable bool dirty_ = true;

        // Local transform relative to parent
        Transform localTransform_;

        // Parent node (weak to avoid cycles)
        std::weak_ptr<TransformNode> parent_;

        // Children nodes
        SharedVec<TransformNode> children_;

        // Internal method to update global transform if dirty
        void updateGlobalTransform() const;
    };

} // namespace spatial
