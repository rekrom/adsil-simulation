#include <adapter/implementations/CarJsonAdapter.hpp>
#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <geometry/implementations/Device.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;
// Küçük farkları tolere eden karşılaştırma
bool floatEqual(float a, float b, float epsilon = 1e-5f)
{
    return std::fabs(a - b) < epsilon;
}

void testCarJsonAdapter()
{
    Point origin(0.0F, 0.0F, 0.0F);
    Vector orientation(0.0F, 0.0F, 0.0F);

    auto node = std::make_shared<spatial::TransformNode>(spatial::Transform(origin, orientation));

    SharedVec<Device> transmitters;
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(0.0F, 0.0F, 0.0F),
                           Vector(-0.7071067F, 0.7071067F, 0.0F)),
        20.0F, 20.0F, 10.0F, "TX1"}));
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(0.0F, 0.0F, 0.0F),
                           Vector(0.7071067F, -0.7071067F, 0.0F)),
        20.0F, 20.0F, 10.0F, "TX2"}));

    SharedVec<Device> receivers;
    receivers.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(1000.0F, 0.0F, 0.0F),
                           Vector(0.0F, 1.0F, 0.0F)),
        20.0F, 20.0F, 10.0F, "RX1"}));
    receivers.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(-1000.0F, 0.0F, 0.0F),
                           Vector(0.0F, 1.0F, 0.0F)),
        20.0F, 20.0F, 10.0F, "RX2"}));
    CarDimension dim(1.0F, 1.0F, 1.0F);
    CarConfig config(node, transmitters, receivers, dim);
    std::shared_ptr<Car> car = std::make_shared<Car>(config);
    CarJsonAdapter adapter;

    // Serialize
    nlohmann::json j = adapter.toJson(car);
    std::cout << "Serialized Car JSON:\n"
              << j.dump(4) << std::endl;

    // Deserialize
    std::shared_ptr<Car> loadedCar = adapter.fromJson(j);

    // Basic validations with tolerance
    assert(floatEqual(loadedCar->getPosition().x(), car->getPosition().x()));
    assert(floatEqual(loadedCar->getPosition().y(), car->getPosition().y()));
    assert(floatEqual(loadedCar->getPosition().z(), car->getPosition().z()));

    assert(floatEqual(loadedCar->getOrientation().x(), car->getOrientation().x()));
    assert(floatEqual(loadedCar->getOrientation().y(), car->getOrientation().y()));
    assert(floatEqual(loadedCar->getOrientation().z(), car->getOrientation().z()));

    assert(loadedCar->getTransmitters().size() == car->getTransmitters().size());
    assert(loadedCar->getReceivers().size() == car->getReceivers().size());

    std::cout << "[PASS] CarJsonAdapter serialization/deserialization test\n";
}

int main()
{
    testCarJsonAdapter();
    return 0;
}
