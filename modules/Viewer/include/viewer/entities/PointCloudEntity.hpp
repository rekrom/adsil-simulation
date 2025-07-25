#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "viewer/entities/Entity.hpp"
#include <math/PointCloud.hpp>
#include <viewer/renderables/PointCloudRenderable.hpp>

namespace viewer
{
    class PointCloudEntity : public Entity
    {
    public:
        PointCloudEntity(std::shared_ptr<math::PointCloud> cloud = nullptr, glm::vec3 color = glm::vec3(0.9F, 0.6F, 0.3F));

        void addPoints(std::vector<math::Point> points);
        void setPointCloud(std::shared_ptr<math::PointCloud> cloud);
        std::shared_ptr<math::PointCloud> getPointCloud() const;

        // void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        glm::vec3 getCenter() const override;
        bool isTransparent() const override;
        std::string getName() const override { return "PointCloudEntity"; }

        void setPointSize(float pointSize);
        void setColor(glm::vec3 color);
        glm::vec3 getColor() const { return renderable_->getColor(); }

        void setAlpha(float alpha);

    private:
        std::shared_ptr<math::PointCloud> cloud_;
        glm::vec3 color_;
    };
}
