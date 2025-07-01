#pragma once

#include <viewer/interfaces/Renderable.hpp>
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

        void setColor(const glm::vec3 &color); // optional
        void setVisible(bool visible);         // toggle display

    private:
        std::shared_ptr<PointCloud> pointCloud_;
        glm::vec3 color_{1.0f, 1.0f, 1.0f}; // default white
        bool visible_{true};

        GLuint vao_{0}, vbo_{0};
        GLuint shader_{0};

        void createShader();
        void createBuffers();
    };

}
