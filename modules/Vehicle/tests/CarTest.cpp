#include <vehicle/Car.hpp>
#include <vehicle/configs/CarConfig.hpp>
#include <spatial/implementations/Transform.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <geometry/implementations/Device.hpp>
#include <geometry/configs/DeviceConfig.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <cassert>
#include <iostream>
#include <memory>

using namespace math;

// Helper function for floating point comparison
bool floatEqual(float a, float b, float epsilon = 1e-5f)
{
    return std::fabs(a - b) < epsilon;
}

void test_Car_default_constructor()
{
    std::cout << "Testing Car default constructor..." << std::endl;

    Car car;

    // Test default dimensions
    CarDimension dimension = car.getDimension();
    assert(floatEqual(dimension.length, Car::DefaultCarDimension.length));
    assert(floatEqual(dimension.width, Car::DefaultCarDimension.width));
    assert(floatEqual(dimension.height, Car::DefaultCarDimension.height));

    // Test that device vectors are empty initially
    assert(car.getTransmitters().empty());
    assert(car.getReceivers().empty());
    assert(car.getAllDevices().empty());

    // Test trajectory initially contains only the start position (global position)
    const auto &trajectory = car.getTrajectory();
    assert(trajectory.size() == 1);
    Point startPosition = car.getGlobalTransform().getPosition();
    assert(floatEqual(trajectory[0].x(), startPosition.x()));
    assert(floatEqual(trajectory[0].y(), startPosition.y()));
    assert(floatEqual(trajectory[0].z(), startPosition.z()));

    std::cout << "✅ Car default constructor test passed" << std::endl;
}

void test_Car_with_config()
{
    std::cout << "Testing Car with configuration..." << std::endl;

    // Create transform node for the car
    spatial::Transform carTransform(Point(0.0f, -1.1f, 0.0f), Vector(0.0f, 1.57f, 0.0f)); // 90 degrees in radians
    auto transformNode = std::make_shared<spatial::TransformNode>(carTransform);

    // Create some test devices (receivers)
    SharedVec<Device> receivers;
    SharedVec<Device> transmitters;

    // Create a receiver at origin
    spatial::Transform rxTransform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    DeviceConfig rxConfig{rxTransform, 120.0f, 120.0f, 1.0f, "rx0"};
    auto receiver = std::make_shared<Device>(rxConfig);
    receivers.push_back(receiver);

    // Create a transmitter
    spatial::Transform txTransform(Point(0.0f, 0.0f, 0.0f), Vector(-0.218f, -0.785f, 0.0f)); // -12.5, -45 degrees
    DeviceConfig txConfig{txTransform, 25.0f, 30.0f, 5.0f, "tx1"};
    auto transmitter = std::make_shared<Device>(txConfig);
    transmitters.push_back(transmitter);

    // Create car dimension
    CarDimension dimension(2.53f, 1.39f, 1.52f);

    // Create car config
    CarConfig config(transformNode, transmitters, receivers, dimension);

    // Create car with config
    Car car(config);

    // Test dimensions
    CarDimension carDim = car.getDimension();
    assert(floatEqual(carDim.length, 2.53f));
    assert(floatEqual(carDim.width, 1.39f));
    assert(floatEqual(carDim.height, 1.52f));

    // Test devices
    assert(car.getReceivers().size() == 1);
    assert(car.getTransmitters().size() == 1);
    assert(car.getAllDevices().size() == 2);

    // Test device names
    assert(car.getReceivers()[0]->getName() == "rx0");
    assert(car.getTransmitters()[0]->getName() == "tx1");

    std::cout << "✅ Car with configuration test passed" << std::endl;
}

void test_Car_dimension_operations()
{
    std::cout << "Testing Car dimension operations..." << std::endl;

    Car car;

    // Test setting custom dimensions
    CarDimension newDimension(4.0f, 2.0f, 1.8f);
    car.setDimension(newDimension);

    CarDimension retrievedDimension = car.getDimension();
    assert(floatEqual(retrievedDimension.length, 4.0f));
    assert(floatEqual(retrievedDimension.width, 2.0f));
    assert(floatEqual(retrievedDimension.height, 1.8f));

    std::cout << "✅ Car dimension operations test passed" << std::endl;
}

void test_Car_movement()
{
    std::cout << "Testing Car movement..." << std::endl;

    // Create a car with initial position
    spatial::Transform initialTransform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    auto transformNode = std::make_shared<spatial::TransformNode>(initialTransform);

    CarDimension dimension(2.0f, 1.0f, 1.0f);
    CarConfig config(transformNode, {}, {}, dimension);
    Car car(config);

    // Test movement
    Point newPosition(5.0f, 10.0f, 2.0f);
    car.moveTo(newPosition);

    // Check trajectory
    const auto &trajectory = car.getTrajectory();
    assert(!trajectory.empty());

    // The trajectory should contain the new position
    Point lastPosition = trajectory.back();
    assert(floatEqual(lastPosition.x(), 5.0f));
    assert(floatEqual(lastPosition.y(), 10.0f));
    assert(floatEqual(lastPosition.z(), 2.0f));

    // Test multiple movements
    car.moveTo(Point(10.0f, 15.0f, 3.0f));
    assert(trajectory.size() >= 2);

    std::cout << "✅ Car movement test passed" << std::endl;
}

void test_Car_multiple_devices()
{
    std::cout << "Testing Car with multiple devices..." << std::endl;

    // Create transform node
    spatial::Transform carTransform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    auto transformNode = std::make_shared<spatial::TransformNode>(carTransform);

    // Create multiple receivers (similar to scene.json)
    SharedVec<Device> receivers;
    SharedVec<Device> transmitters;

    // Receiver at origin
    spatial::Transform rx0Transform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    DeviceConfig rx0Config{rx0Transform, 120.0f, 120.0f, 1.0f, "rx0"};
    receivers.push_back(std::make_shared<Device>(rx0Config));

    // Receiver on the right
    spatial::Transform rx1Transform(Point(1.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    DeviceConfig rx1Config{rx1Transform, 120.0f, 120.0f, 1.0f, "rx1"};
    receivers.push_back(std::make_shared<Device>(rx1Config));

    // Receiver on the left
    spatial::Transform rx2Transform(Point(-1.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    DeviceConfig rx2Config{rx2Transform, 120.0f, 120.0f, 1.0f, "rx2"};
    receivers.push_back(std::make_shared<Device>(rx2Config));

    // Receiver at front
    spatial::Transform rx3Transform(Point(0.0f, 1.26f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    DeviceConfig rx3Config{rx3Transform, 120.0f, 120.0f, 1.0f, "rx3"};
    receivers.push_back(std::make_shared<Device>(rx3Config));

    // Multiple transmitters (similar to scene.json)
    spatial::Transform tx1Transform(Point(0.0f, 0.0f, 0.0f), Vector(-0.218f, -0.785f, 0.0f));
    DeviceConfig tx1Config{tx1Transform, 25.0f, 30.0f, 5.0f, "tx1"};
    transmitters.push_back(std::make_shared<Device>(tx1Config));

    spatial::Transform tx2Transform(Point(0.0f, 0.0f, 0.0f), Vector(-0.218f, -0.262f, 0.0f));
    DeviceConfig tx2Config{tx2Transform, 25.0f, 30.0f, 5.0f, "tx2"};
    transmitters.push_back(std::make_shared<Device>(tx2Config));

    // Create car config
    CarDimension dimension(2.53f, 1.39f, 1.52f);
    CarConfig config(transformNode, transmitters, receivers, dimension);
    Car car(config);

    // Test device counts
    assert(car.getReceivers().size() == 4);
    assert(car.getTransmitters().size() == 2);
    assert(car.getAllDevices().size() == 6);

    // Test device names
    assert(car.getReceivers()[0]->getName() == "rx0");
    assert(car.getReceivers()[1]->getName() == "rx1");
    assert(car.getReceivers()[2]->getName() == "rx2");
    assert(car.getReceivers()[3]->getName() == "rx3");
    assert(car.getTransmitters()[0]->getName() == "tx1");
    assert(car.getTransmitters()[1]->getName() == "tx2");

    std::cout << "✅ Car with multiple devices test passed" << std::endl;
}

void test_Car_toString()
{
    std::cout << "Testing Car toString method..." << std::endl;

    Car car;

    // Test that toString returns a non-empty string
    std::string carString = car.toString();
    assert(!carString.empty());

    // Test that the string contains some expected information
    // (This will depend on the actual implementation)

    std::cout << "✅ Car toString test passed" << std::endl;
}

void test_CarDimension_struct()
{
    std::cout << "Testing CarDimension struct..." << std::endl;

    // Test default constructor
    CarDimension defaultDim;
    assert(floatEqual(defaultDim.length, 0.0f));
    assert(floatEqual(defaultDim.width, 0.0f));
    assert(floatEqual(defaultDim.height, 0.0f));

    // Test parameterized constructor
    CarDimension customDim(5.0f, 2.5f, 1.8f);
    assert(floatEqual(customDim.length, 5.0f));
    assert(floatEqual(customDim.width, 2.5f));
    assert(floatEqual(customDim.height, 1.8f));

    // Test default car dimension constant
    assert(floatEqual(Car::DefaultCarDimension.length, 2.53f));
    assert(floatEqual(Car::DefaultCarDimension.width, 1.39f));
    assert(floatEqual(Car::DefaultCarDimension.height, 1.52f));

    std::cout << "✅ CarDimension struct test passed" << std::endl;
}

int main()
{
    std::cout << "🧪 Running Vehicle Module Tests..." << std::endl;

    try
    {
        test_CarDimension_struct();
        test_Car_default_constructor();
        test_Car_with_config();
        test_Car_dimension_operations();
        test_Car_movement();
        test_Car_multiple_devices();
        test_Car_toString();

        std::cout << "✅ All Vehicle module tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
