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
    Point origin(0.0f, 0.0f, 0.0f);
    Vector orientation(0.0f, 0.0f, 0.0f);

    auto node = std::make_shared<spatial::TransformNode>(Transform(origin, orientation));

    std::vector<std::shared_ptr<Device>> transmitters;
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        Transform(Point(0.0f, 0.0f, 0.0f),
                  Vector(-0.7071067f, 0.7071067f, 0.0f)),
        20.0f, 20.0f, "TX1"}));
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        Transform(Point(0.0f, 0.0f, 0.0f),
                  Vector(0.7071067f, -0.7071067f, 0.0f)),
        20.0f, 20.0f, "TX2"}));

    std::vector<std::shared_ptr<Device>> receivers;
    receivers.push_back(std::make_shared<Device>(DeviceConfig{
        Transform(Point(1000.0f, 0.0f, 0.0f),
                  Vector(0.0f, 1.0f, 0.0f)),
        20.0f, 20.0f, "RX1"}));
    receivers.push_back(std::make_shared<Device>(DeviceConfig{
        Transform(Point(-1000.0f, 0.0f, 0.0f),
                  Vector(0.0f, 1.0f, 0.0f)),
        20.0f, 20.0f, "RX2"}));
    CarDimension dim(1.0f, 1.0f, 1.0f);
    CarConfig config(node, transmitters, receivers, dim);
    Car car(config);

    CarJsonAdapter adapter;

    // Serialize
    nlohmann::json j = adapter.toJson(car);
    std::cout << "Serialized Car JSON:\n"
              << j.dump(4) << std::endl;

    // Deserialize
    Car loadedCar = adapter.fromJson(j);

    // Basic validations with tolerance
    assert(floatEqual(loadedCar.getPosition().x(), car.getPosition().x()));
    assert(floatEqual(loadedCar.getPosition().y(), car.getPosition().y()));
    assert(floatEqual(loadedCar.getPosition().z(), car.getPosition().z()));

    assert(floatEqual(loadedCar.getOrientation().x(), car.getOrientation().x()));
    assert(floatEqual(loadedCar.getOrientation().y(), car.getOrientation().y()));
    assert(floatEqual(loadedCar.getOrientation().z(), car.getOrientation().z()));

    assert(loadedCar.getTransmitters().size() == car.getTransmitters().size());
    assert(loadedCar.getReceivers().size() == car.getReceivers().size());

    std::cout << "[PASS] CarJsonAdapter serialization/deserialization test\n";
}

int main()
{
    testCarJsonAdapter();
    return 0;
}
