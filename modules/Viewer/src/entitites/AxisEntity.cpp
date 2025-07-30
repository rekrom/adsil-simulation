// AxisEntity.cpp
#include <viewer/entities/AxisEntity.hpp>
#include <iostream>
namespace viewer
{
    AxisEntity::AxisEntity()
    {
        renderable_ = std::make_shared<AxisRenderable>();
        setName("AxisEntity");
    }
}
