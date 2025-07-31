#include "main.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    auto sim = std::make_shared<simulation::SimulationManager>();
    sim->run();
    return 0;
}// Test change Thu Jul 31 01:03:26 PM +03 2025
// Performance test change - 1753956220
