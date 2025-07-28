// GroundEntity.cpp
#include <viewer/entities/GroundEntity.hpp>
#include <iostream>
namespace viewer
{
    GroundEntity::GroundEntity()
    {
        renderable_ = std::make_shared<GroundRenderable>();
    }

}
