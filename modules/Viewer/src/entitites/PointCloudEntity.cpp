#include "viewer/entities/PointCloudEntity.hpp"

namespace viewer
{

    PointCloudEntity::PointCloudEntity(std::shared_ptr<PointCloud> cloud, glm::vec3 color)
        : cloud_(cloud ? cloud : std::make_shared<PointCloud>()), color_(color)
    {
        renderable_ = std::make_shared<PointCloudRenderable>(cloud_, color_);
    }

    void PointCloudEntity::setPointCloud(std::shared_ptr<PointCloud> cloud)
    {
        cloud_ = cloud;
        if (renderable_)
        {
            renderable_->updatePointCloud(cloud);
        }
    }

    std::shared_ptr<PointCloud> PointCloudEntity::getPointCloud() const
    {
        return cloud_;
    }

    void PointCloudEntity::initGL()
    {
        if (renderable_)
            renderable_->initGL();
    }

    void PointCloudEntity::render(const glm::mat4 &view, const glm::mat4 &projection)
    {

        if (renderable_)
        {
            renderable_->render(view, projection);
        }
    }

    void PointCloudEntity::cleanup()
    {
        if (renderable_)
            renderable_->cleanup();
    }

    glm::vec3 PointCloudEntity::getColor() const
    {
        return color_;
    }

    glm::vec3 PointCloudEntity::getCenter() const
    {
        return glm::vec3(0.0F); // or compute cloud center if needed
    }

    bool PointCloudEntity::isTransparent() const
    {
        return false; // or configurable
    }

    std::string PointCloudEntity::getName() const
    {
        return "PointCloudEntity";
    }

}
