#include <adapter/AdapterManager.hpp>
#include <adapter/implementations/CarJsonAdapter.hpp>
#include <adapter/implementations/SceneJsonAdapter.hpp>
#include <adapter/implementations/FrameJsonAdapter.hpp>
#include <simulation/implementations/Frame.hpp>

namespace adapter
{

    AdapterManager::AdapterManager()
    {
        registry_.registerAdapter<std::shared_ptr<Car>>(std::make_shared<CarJsonAdapter>());
        registry_.registerAdapter<std::shared_ptr<SimulationScene>>(std::make_shared<SceneJsonAdapter>());
        registry_.registerAdapter<std::shared_ptr<simulation::Frame>>(std::make_shared<FrameJsonAdapter>());
        // You can register more adapters here as needed
    }

}
