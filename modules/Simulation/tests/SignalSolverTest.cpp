// Basic unit tests for SignalSolver focusing on edge cases and preconditions.
// These tests avoid any OpenGL context creation by exercising only pure logic paths.

#include <simulation/SignalSolver.hpp>
#include <simulation/SimulationScene.hpp>
#include <geometry/configs/DeviceConfig.hpp>
#include <geometry/implementations/Device.hpp>
#include <vehicle/Car.hpp>
#include <vehicle/configs/CarConfig.hpp>
#include <spatial/implementations/Transform.hpp>
#include <math/PointCloud.hpp>
#include <math/Point.hpp>

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <stdexcept>

// Simple test framework
class SimpleTest
{
public:
    static void assert_true(bool condition, const std::string &message)
    {
        if (!condition)
        {
            std::cerr << "[FAIL] " << message << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_equal_int(int expected, int actual, const std::string &message)
    {
        if (expected != actual)
        {
            std::cerr << "[FAIL] " << message << " - Expected: " << expected << ", Actual: " << actual << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_equal_float(float expected, float actual, const std::string &message, float tolerance = 0.001f)
    {
        if (std::abs(expected - actual) > tolerance)
        {
            std::cerr << "[FAIL] " << message << " - Expected: " << expected << ", Actual: " << actual << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_equal_string(const std::string &expected, const std::string &actual, const std::string &message)
    {
        if (expected != actual)
        {
            std::cerr << "[FAIL] " << message << " - Expected: " << expected << ", Actual: " << actual << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }
};

// Helper to create a device with wide FOV aimed along +X
static std::shared_ptr<Device> makeDevice(const std::string &name, const math::Point &pos)
{
    DeviceConfig cfg{spatial::Transform(pos, {0.0F, 0.0F, 0.0F}),
                     120.0F, 120.0F, 100.0F, name};
    return std::make_shared<Device>(cfg);
}

// Builds a car with 1 transmitter and N receivers
static std::shared_ptr<Car> buildCar(std::shared_ptr<Device> tx, const SharedVec<Device> &receivers)
{
    auto carNode = std::make_shared<spatial::TransformNode>();
    SharedVec<Device> txs{tx};
    CarConfig cfg(carNode, txs, receivers, Car::DefaultCarDimension);
    return std::make_shared<Car>(cfg);
}

static void test_emptyPointCloud_returnsEmpty()
{
    std::cout << "\n=== test_emptyPointCloud_returnsEmpty ===" << std::endl;
    auto scene = std::make_shared<SimulationScene>();
    auto tx = makeDevice("tx", {0, 0, 0});
    // Need receivers for solver to examine but leave cloud empty
    SharedVec<Device> rxs{makeDevice("rx1", {1, 0, 0}), makeDevice("rx2", {0, 1, 0}), makeDevice("rx3", {0, 0, 1}), makeDevice("rx4", {1, 1, 0})};
    scene->setCar(buildCar(tx, rxs));
    auto solver = simulation::SignalSolver(scene);
    auto result = solver.solve();
    SimpleTest::assert_true(result->empty(), "Solver returns empty cloud when input cloud empty");
}

static void test_noReceivers_returnsEmpty()
{
    std::cout << "\n=== test_noReceivers_returnsEmpty ===" << std::endl;
    auto scene = std::make_shared<SimulationScene>();
    auto tx = makeDevice("tx", {0, 0, 0});
    SharedVec<Device> noReceivers; // empty
    scene->setCar(buildCar(tx, noReceivers));
    // Provide a point cloud but with no receivers nothing should be processed
    auto cloud = std::make_shared<math::PointCloud>();
    cloud->addPoint({5, 0, 0});
    scene->setExternalPointCloud(cloud);
    auto solver = simulation::SignalSolver(scene);
    auto result = solver.solve();
    SimpleTest::assert_true(result->empty(), "Solver returns empty when there are no receivers");
}

static void test_lessThanFourReceivers_trilaterationException()
{
    std::cout << "\n=== test_lessThanFourReceivers_trilaterationException ===" << std::endl;
    auto scene = std::make_shared<SimulationScene>();
    auto tx = makeDevice("tx", {0, 0, 0});
    SharedVec<Device> rxs{makeDevice("rx1", {3, 0, 0}), makeDevice("rx2", {0, 3, 0}), makeDevice("rx3", {0, 0, 3})}; // only 3
    scene->setCar(buildCar(tx, rxs));
    auto cloud = std::make_shared<math::PointCloud>();
    cloud->addPoint({5, 0, 0}); // inside wide FOVs
    scene->setExternalPointCloud(cloud);
    auto solver = simulation::SignalSolver(scene);
    bool threw = false;
    try
    {
        (void)solver.solve();
    }
    catch (const std::runtime_error &e)
    {
        threw = std::string(e.what()).find("ADSIL requires exactly 4 receivers") != std::string::npos;
    }
    SimpleTest::assert_true(threw, "Solver throws when receiver count != 4 and trilateration attempted");
}

static void test_fourReceivers_withPoint_noException()
{
    std::cout << "\n=== test_fourReceivers_withPoint_noException ===" << std::endl;
    auto scene = std::make_shared<SimulationScene>();
    auto tx = makeDevice("tx", {0, 0, 0});
    SharedVec<Device> rxs{makeDevice("rx1", {3, 0, 0}), makeDevice("rx2", {0, 3, 0}), makeDevice("rx3", {0, 0, 3}), makeDevice("rx4", {2, 2, 0})};
    scene->setCar(buildCar(tx, rxs));
    auto cloud = std::make_shared<math::PointCloud>();
    cloud->addPoint({5, 0, 0});
    scene->setExternalPointCloud(cloud);
    auto solver = simulation::SignalSolver(scene);
    bool ok = true;
    std::shared_ptr<math::PointCloud> result;
    try
    {
        result = solver.solve();
    }
    catch (const std::exception &e)
    {
        ok = false;
        std::cout << "[INFO] Unexpected exception: " << e.what() << std::endl;
    }
    SimpleTest::assert_true(ok, "Solver does not throw with 4 receivers");
    // We don't assert specific ADSIL output (depends on geometry), just that a result object exists
    SimpleTest::assert_true(result != nullptr, "Result point cloud created");
}

// ================= Deterministic fixture test (predefined point cloud) =================
// We capture the solver's current ADSIL trilateration output for a simple controlled
// geometry so future math changes can be detected. This is added after the generic tests.

static void test_deterministic_single_point_fixture()
{
    std::cout << "\n=== test_deterministic_single_point_fixture ===" << std::endl;
    auto scene = std::make_shared<SimulationScene>();
    // One transmitter at origin
    auto tx = makeDevice("tx", {0, 0, 0});
    // Four receivers forming a non-degenerate tetra-like base around origin
    SharedVec<Device> rxs{
        makeDevice("rx0", {3.0f, 0.0f, 0.0f}),
        makeDevice("rx1", {0.0f, 3.0f, 0.0f}),
        makeDevice("rx2", {0.0f, 0.0f, 3.0f}),
        makeDevice("rx3", {2.0f, 2.0f, 2.0f})};
    scene->setCar(buildCar(tx, rxs));

    // Predefined external point cloud with a single point
    auto cloud = std::make_shared<math::PointCloud>();
    cloud->addPoint({5.0f, 0.0f, 0.0f});
    scene->setExternalPointCloud(cloud);

    simulation::SignalSolver solver(scene);
    auto result = solver.solve();

    // For first run, just print observed points. After observing stable output, we can lock expectations.
    std::cout << "Observed ADSIL result point count: " << result->getPoints().size() << std::endl;
    for (const auto &p : result->getPoints())
    {
        std::cout << "ResultPoint: " << p.toString() << std::endl;
    }

    // Placeholder assertions: ensure we got at least one solution point.
    SimpleTest::assert_true(!result->empty(), "Deterministic fixture produced at least one ADSIL point");
    // Future enhancement: lock coordinates with tolerance once stabilized.
}

// Extend main to run deterministic test last so its printed output is easy to capture.

int main()
{
    std::cout << "SignalSolver Test Suite" << std::endl;
    std::cout << "===========================" << std::endl;

    try
    {
        test_emptyPointCloud_returnsEmpty();
        test_noReceivers_returnsEmpty();
        test_lessThanFourReceivers_trilaterationException();
        test_fourReceivers_withPoint_noException();
        test_deterministic_single_point_fixture();

        std::cout << "\n=== All Tests Passed ===" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
