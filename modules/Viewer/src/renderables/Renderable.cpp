#include <viewer/renderables/Renderable.hpp>

namespace viewer
{

    Renderable::Renderable()
    {
    }

    Renderable::~Renderable() = default;

    float Renderable::getAlpha() const
    {
        return alpha_;
    }

    void Renderable::setAlpha(float alpha)
    {
        alpha_ = alpha;
    }

    glm::vec3 Renderable::getColor() const
    {
        return color_;
    }

    void Renderable::setColor(glm::vec3 color)
    {
        color_ = color;
    }

    bool Renderable::isTransparent() const
    {
        return alpha_ < 1.0F;
    }

    void Renderable::createBuffers() {}
    void Renderable::createShader() {}

} // namespace viewer
