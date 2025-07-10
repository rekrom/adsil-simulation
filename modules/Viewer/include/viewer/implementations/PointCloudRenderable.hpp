#pragma once

#include <viewer/implementations/Renderable.hpp>
#include <core/PointCloud.hpp>
#include <memory>
#include <glad/glad.h>

namespace viewer
{
    class PointCloudRenderable : public Renderable
    {
    public:
        explicit PointCloudRenderable(std::shared_ptr<PointCloud> pointCloud);
        ~PointCloudRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

    private:
        std::shared_ptr<PointCloud> pointCloud_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
