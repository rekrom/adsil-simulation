#pragma once
#include <core/Logger.hpp>

#include <viewer/renderables/Renderable.hpp>
#include <math/Point.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace viewer
{
    class WireframeRenderable : public Renderable
    {
    public:
        explicit WireframeRenderable(const std::vector<math::Point> &lines, glm::vec3 color = glm::vec3(1.0F));
        ~WireframeRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

    private:
        std::vector<math::Point> lines_;
        glm::vec3 color_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
