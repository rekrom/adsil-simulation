#pragma once

#include <viewer/Renderable.hpp>
#include <core/Point.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class WireframeRenderable : public Renderable
{
public:
    explicit WireframeRenderable(const std::vector<Point> &lines, glm::vec3 color = glm::vec3(1.0f));
    ~WireframeRenderable();

    void initGL() override;
    void render(const glm::mat4 &view, const glm::mat4 &projection) override;

private:
    void createBuffers();
    void createShader();

    std::vector<Point> lines_;
    glm::vec3 color_;
    GLuint vao_{0}, vbo_{0}, shader_{0};
};
