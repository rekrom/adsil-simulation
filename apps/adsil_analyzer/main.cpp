#include "main.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    auto sim = std::make_shared<simulation::SimulationManager>();
    sim->run();
    return 0;
}