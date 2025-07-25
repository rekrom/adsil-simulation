#include "viewer/entities/PointCloudEntity.hpp"

namespace viewer
{

    PointCloudEntity::PointCloudEntity(std::shared_ptr<math::PointCloud> cloud, glm::vec3 color)
        : cloud_(cloud ? cloud : std::make_shared<math::PointCloud>()), color_(color)
    {
        renderable_ = std::make_shared<PointCloudRenderable>(cloud_, color_);
    }

    void PointCloudEntity::addPoints(std::vector<math::Point> points)
    {
        if (!cloud_)
        {
            cloud_ = std::make_shared<math::PointCloud>();
        }
        cloud_->addPoints(points);
        if (renderable_)
        {
            std::dynamic_pointer_cast<PointCloudRenderable>(renderable_)->updatePointCloud(cloud_);
        }
    }

    void PointCloudEntity::setPointCloud(std::shared_ptr<math::PointCloud> cloud)
    {
        cloud_ = cloud;
        if (renderable_)
        {
            std::dynamic_pointer_cast<PointCloudRenderable>(renderable_)->updatePointCloud(cloud);
        }
    }

    std::shared_ptr<math::PointCloud> PointCloudEntity::getPointCloud() const
    {
        return cloud_;
    }

    // void PointCloudEntity::initGL()
    // {
    //     if (renderable_)
    //         renderable_->initGL();
    // }

    void PointCloudEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!visible_ || !renderable_)
        {
            return;
        }

        renderable_->render(view, projection);
    }

    void PointCloudEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();
    }

    bool PointCloudEntity::isTransparent() const
    {
        if (!renderable_)
        {
            return false;
        }
        return renderable_->isTransparent();
    }

    void PointCloudEntity::setPointSize(float pointSize)
    {
        std::dynamic_pointer_cast<PointCloudRenderable>(renderable_)->setPointSize(pointSize);
    }

    void PointCloudEntity::setAlpha(float alpha)
    {
        renderable_->setAlpha(alpha);
    }

    void PointCloudEntity::setColor(glm::vec3 color)
    {
        renderable_->setColor(color);
    }

    glm::vec3 PointCloudEntity::getCenter() const
    {
        if (renderable_)
        {
            LOGGER_ERROR("PointCloudEntity::getCenter() called, but this method is deprecated. Use renderable_->getCenter() instead.");

            // return renderable_->getCenter();
        }
        return glm::vec3(0.0f);
    }

}
