#pragma once

#include <viewer/renderables/Renderable.hpp>
#include <math/PointCloud.hpp>
#include <core/Logger.hpp>
#include <memory>
#include <glad/glad.h>

namespace viewer
{
    class PointCloudRenderable : public Renderable
    {
    public:
        explicit PointCloudRenderable(std::shared_ptr<math::PointCloud> pointCloud, glm::vec3 color);
        ~PointCloudRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        void updatePointCloud(std::shared_ptr<math::PointCloud> newCloud);

        void updateBuffers();

        void setPointSize(float pointSize);

        std::size_t getPointCloudSize() const
        {
            if (pointCloud_)
            {
                return pointCloud_->size();
            }
            return 0;
        }

    private:
        bool dirty_ = true;
        std::shared_ptr<math::PointCloud> pointCloud_;
        float pointSize_ = 1.0F;
        std::vector<float> vertices_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
