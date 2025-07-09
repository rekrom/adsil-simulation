#include "main.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    // // SignalSolver solver(scene);
    // // auto closestPoints = solver.solve();
    // // std::cout << closestPoints->toString() << std::endl;

    // // auto closestPointRenderable = std::make_shared<PointCloudRenderable>(closestPoints);
    // // closestPointRenderable->setColor(glm::vec3(1.0f, 0.0f, 0.5f));
    // // closestPointRenderable->setVisible(true);
    // // viewer.addRenderable(closestPointRenderable);

    simulation::SimulationApp sim;
    sim.run();
    return 0;
}