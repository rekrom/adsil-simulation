#include "main.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    // // SignalSolver solver(scene);
    // // auto closestPoints = solver.solve();
    // // std::cout << closestPoints->toString() << std::endl;

    // // auto closestPointRenderable = std::make_shared<PointCloudRenderable>(closestPoints);
    // // closestPointRenderable->setColor(glm::vec3(1.0F, 0.0F, 0.5F));
    // // closestPointRenderable->setVisible(true);
    // // viewer.addRenderable(closestPointRenderable);

    simulation::SimulationManager sim;
    sim.run();
    return 0;
}