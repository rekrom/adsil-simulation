#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "viewer/entities/Entity.hpp"
#include "core/PointCloud.hpp"
#include "viewer/implementations/PointCloudRenderable.hpp"

namespace viewer
{
    class PointCloudEntity : public Entity
    {
    public:
        PointCloudEntity(std::shared_ptr<PointCloud> cloud = nullptr, glm::vec3 color = glm::vec3(0.9F, 0.6F, 0.3F));

        void setPointCloud(std::shared_ptr<PointCloud> cloud);
        std::shared_ptr<PointCloud> getPointCloud() const;

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        glm::vec3 getColor() const override;
        glm::vec3 getCenter() const override;
        bool isTransparent() const override;
        std::string getName() const override;

    private:
        std::shared_ptr<PointCloud> cloud_;
        std::shared_ptr<PointCloudRenderable> renderable_;
        glm::vec3 color_;
    };
}
