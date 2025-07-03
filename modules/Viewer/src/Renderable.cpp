#include <viewer/implementations/Renderable.hpp>

namespace viewer
{

    Renderable::Renderable()
        : alpha_(1.0f) {}

    Renderable::~Renderable() = default;

    float Renderable::getAlpha() const
    {
        return alpha_;
    }

    void Renderable::setAlpha(float alpha)
    {
        alpha_ = alpha;
    }

    bool Renderable::isTransparent() const
    {
        return alpha_ < 1.0f;
    }

    void Renderable::createBuffers() {}
    void Renderable::createShader() {}

} // namespace viewer
