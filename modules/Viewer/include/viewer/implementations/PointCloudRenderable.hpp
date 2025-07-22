#pragma once

#include <viewer/implementations/Renderable.hpp>
#include <core/PointCloud.hpp>
#include <core/Logger.hpp>
#include <memory>
#include <glad/glad.h>

namespace viewer
{
    class PointCloudRenderable : public Renderable
    {
    public:
        explicit PointCloudRenderable(std::shared_ptr<PointCloud> pointCloud, glm::vec3 color);
        ~PointCloudRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        void updatePointCloud(std::shared_ptr<PointCloud> newCloud);

        void updateBuffers();

        void setPointSize(float pointSize);

    private:
        bool dirty_ = true;
        std::shared_ptr<PointCloud> pointCloud_;
        float pointSize_ = 1.0F;
        std::vector<float> vertices_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
