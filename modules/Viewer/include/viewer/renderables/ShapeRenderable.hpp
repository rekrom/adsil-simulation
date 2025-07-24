#pragma once
#include <core/Logger.hpp>

#include <geometry/implementations/ShapeBase.hpp>
#include <viewer/renderables/Renderable.hpp>
#include <memory>
#include <glm/glm.hpp>

namespace viewer
{

    class ShapeRenderable : public Renderable
    {
    public:
        explicit ShapeRenderable(std::shared_ptr<ShapeBase> shape,
                                 const glm::vec3 &color = glm::vec3(0.6F, 0.6F, 0.9F));
        ~ShapeRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;
        glm::vec3 getCenter() const override;

    protected:
        void createBuffers() override;
        void createShader() override;

    private:
        std::shared_ptr<ShapeBase> shape_;
        size_t vertexCount_ = 0;
    };

}
