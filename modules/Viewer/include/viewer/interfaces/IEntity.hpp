#pragma once

#include <glm/glm.hpp>
#include <string>

namespace viewer
{
    class IEntity
    {
    public:
        virtual ~IEntity() = default;

        virtual void initGL() = 0;
        virtual void render(const glm::mat4 &view, const glm::mat4 &projection) = 0;
        virtual void cleanup() = 0;

        virtual bool isVisible() const = 0;
        virtual void setVisible(bool visible) = 0;

        virtual bool isTransparent() const = 0;

        virtual glm::vec3 getCenter() const = 0;           // to fix the depth sorting issue
        virtual std::string getName() const = 0;           // to get the name of the entity
        virtual void setName(const std::string &name) = 0; // to set the name of the entity
    };
}