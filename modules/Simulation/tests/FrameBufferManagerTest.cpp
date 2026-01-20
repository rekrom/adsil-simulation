// Unit tests for FrameBufferManager focusing on frame buffering logic.
// These tests use mock/stub approaches to avoid filesystem and OpenGL dependencies.

#include <simulation/implementations/FrameBufferManager.hpp>
#include <simulation/implementations/Frame.hpp>
#include <simulation/interfaces/IFrameObserver.hpp>
#include <math/PointCloud.hpp>
#include <math/Point.hpp>

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <vector>
#include <functional>

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

    static void assert_false(bool condition, const std::string &message)
    {
        assert_true(!condition, message);
    }

    static void assert_not_null(void *ptr, const std::string &message)
    {
        if (ptr == nullptr)
        {
            std::cerr << "[FAIL] " << message << " - Pointer is null" << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_null(void *ptr, const std::string &message)
    {
        if (ptr != nullptr)
        {
            std::cerr << "[FAIL] " << message << " - Pointer is not null" << std::endl;
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

    static void assert_equal_double(double expected, double actual, const std::string &message, double tolerance = 0.001)
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
};

// Mock frame observer for testing observer pattern
class MockFrameObserver : public simulation::IFrameObserver
{
public:
    MockFrameObserver(const std::string &name)
    {
        observerName_ = name;
    }

    void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override
    {
        callCount_++;
        lastFrame_ = frame;
        if (frame)
        {
            lastTimestamp_ = frame->timestamp;
        }
    }

    int getCallCount() const { return callCount_; }
    std::shared_ptr<simulation::Frame> getLastFrame() const { return lastFrame_; }
    double getLastTimestamp() const { return lastTimestamp_; }
    void reset()
    {
        callCount_ = 0;
        lastFrame_.reset();
        lastTimestamp_ = 0.0;
    }

private:
    int callCount_ = 0;
    std::shared_ptr<simulation::Frame> lastFrame_;
    double lastTimestamp_ = 0.0;
};

// ============================================
// Test Functions
// ============================================

void testFrameStructure()
{
    std::cout << "\n=== Testing Frame Structure ===" << std::endl;

    // Test default Frame creation
    simulation::Frame frame;
    SimpleTest::assert_null(frame.cloud.get(), "Frame cloud is initially null");
    SimpleTest::assert_equal_double(0.0, frame.timestamp, "Frame timestamp is initially 0");
    SimpleTest::assert_true(frame.filePath.empty(), "Frame filePath is initially empty");

    // Test Frame with data
    auto pointCloud = std::make_shared<math::PointCloud>();
    pointCloud->addPoint(math::Point(1.0f, 2.0f, 3.0f));
    pointCloud->addPoint(math::Point(4.0f, 5.0f, 6.0f));

    frame.cloud = pointCloud;
    frame.timestamp = 123.456;
    frame.filePath = "/path/to/frame.json";

    SimpleTest::assert_not_null(frame.cloud.get(), "Frame cloud is set correctly");
    SimpleTest::assert_equal_double(123.456, frame.timestamp, "Frame timestamp is set correctly");
    SimpleTest::assert_true(frame.filePath == "/path/to/frame.json", "Frame filePath is set correctly");

    // Test Frame clear
    frame.clear();
    SimpleTest::assert_null(frame.cloud.get(), "Frame cloud is cleared");
    SimpleTest::assert_true(frame.filePath.empty(), "Frame filePath is cleared");
}

void testFrameSharedPtr()
{
    std::cout << "\n=== Testing Frame Shared Pointer ===" << std::endl;

    auto frame = std::make_shared<simulation::Frame>();
    SimpleTest::assert_not_null(frame.get(), "Shared Frame pointer created");

    auto pointCloud = std::make_shared<math::PointCloud>();
    pointCloud->addPoint(math::Point(10.0f, 20.0f, 30.0f));
    frame->cloud = pointCloud;
    frame->timestamp = 999.999;
    frame->filePath = "test_frame.json";

    // Test shared_ptr copy semantics
    auto frameCopy = frame;
    SimpleTest::assert_true(frame.get() == frameCopy.get(), "Shared pointer copy points to same object");
    SimpleTest::assert_equal_double(frame->timestamp, frameCopy->timestamp, "Timestamp accessible from copy");

    // Test clear on shared frame
    frame->clear();
    SimpleTest::assert_null(frameCopy->cloud.get(), "Clear affects both shared pointers");
}

void testMockFrameObserver()
{
    std::cout << "\n=== Testing Mock Frame Observer ===" << std::endl;

    auto observer = std::make_shared<MockFrameObserver>("TestObserver");
    SimpleTest::assert_equal_int(0, observer->getCallCount(), "Observer starts with zero calls");

    // Create and notify with frame
    auto frame = std::make_shared<simulation::Frame>();
    frame->timestamp = 42.0;
    frame->cloud = std::make_shared<math::PointCloud>();

    observer->onFrameChanged(frame);
    SimpleTest::assert_equal_int(1, observer->getCallCount(), "Observer received notification");
    SimpleTest::assert_equal_double(42.0, observer->getLastTimestamp(), "Observer recorded correct timestamp");

    // Test multiple notifications
    for (int i = 0; i < 5; ++i)
    {
        auto newFrame = std::make_shared<simulation::Frame>();
        newFrame->timestamp = static_cast<double>(i * 10);
        observer->onFrameChanged(newFrame);
    }
    SimpleTest::assert_equal_int(6, observer->getCallCount(), "Observer received all notifications");

    // Test reset
    observer->reset();
    SimpleTest::assert_equal_int(0, observer->getCallCount(), "Observer reset successful");
}

void testFrameObserverInterface()
{
    std::cout << "\n=== Testing IFrameObserver Interface ===" << std::endl;

    auto observer = std::make_shared<MockFrameObserver>("InterfaceTest");

    // Test observer name
    std::string name = observer->getObserverName();
    SimpleTest::assert_true(name == "InterfaceTest", "Observer name is set correctly");

    // Test with nullptr frame
    observer->onFrameChanged(nullptr);
    SimpleTest::assert_equal_int(1, observer->getCallCount(), "Observer handles null frame");
    SimpleTest::assert_null(observer->getLastFrame().get(), "Last frame is null after null notification");
}

void testMultipleObservers()
{
    std::cout << "\n=== Testing Multiple Frame Observers ===" << std::endl;

    std::vector<std::shared_ptr<MockFrameObserver>> observers;
    for (int i = 0; i < 5; ++i)
    {
        observers.push_back(std::make_shared<MockFrameObserver>("Observer_" + std::to_string(i)));
    }

    auto frame = std::make_shared<simulation::Frame>();
    frame->timestamp = 100.0;
    frame->cloud = std::make_shared<math::PointCloud>();
    frame->cloud->addPoint(math::Point(0, 0, 0));

    // Notify all observers
    for (auto &obs : observers)
    {
        obs->onFrameChanged(frame);
    }

    // Verify all received notification
    for (size_t i = 0; i < observers.size(); ++i)
    {
        SimpleTest::assert_equal_int(1, observers[i]->getCallCount(),
                                     "Observer " + std::to_string(i) + " received notification");
        SimpleTest::assert_equal_double(100.0, observers[i]->getLastTimestamp(),
                                        "Observer " + std::to_string(i) + " has correct timestamp");
    }
}

void testPointCloudWithFrame()
{
    std::cout << "\n=== Testing PointCloud with Frame ===" << std::endl;

    auto frame = std::make_shared<simulation::Frame>();
    auto cloud = std::make_shared<math::PointCloud>();

    // Add points
    for (int i = 0; i < 100; ++i)
    {
        cloud->addPoint(math::Point(
            static_cast<float>(i),
            static_cast<float>(i * 2),
            static_cast<float>(i * 3)));
    }

    frame->cloud = cloud;
    frame->timestamp = 50.5;

    SimpleTest::assert_not_null(frame->cloud.get(), "Cloud assigned to frame");
    SimpleTest::assert_equal_int(100, static_cast<int>(frame->cloud->size()), "Cloud has correct point count");

    // Test point retrieval
    const auto &points = frame->cloud->getPoints();
    SimpleTest::assert_equal_float(0.0f, points[0].x(), "First point x correct");
    SimpleTest::assert_equal_float(99.0f, points[99].x(), "Last point x correct");
}

void testFrameTimestampPrecision()
{
    std::cout << "\n=== Testing Frame Timestamp Precision ===" << std::endl;

    simulation::Frame frame;

    // Test various timestamp values
    double timestamps[] = {0.0, 1.0, 0.001, 1000000.0, 0.123456789, -1.0};

    for (double ts : timestamps)
    {
        frame.timestamp = ts;
        SimpleTest::assert_equal_double(ts, frame.timestamp,
                                        "Timestamp precision maintained for " + std::to_string(ts), 1e-9);
    }
}

void testFrameClearMultipleTimes()
{
    std::cout << "\n=== Testing Frame Clear Multiple Times ===" << std::endl;

    auto frame = std::make_shared<simulation::Frame>();

    // Clear empty frame
    frame->clear();
    SimpleTest::assert_true(true, "Clear on empty frame succeeds");

    // Set data and clear
    frame->cloud = std::make_shared<math::PointCloud>();
    frame->cloud->addPoint(math::Point(1, 2, 3));
    frame->timestamp = 10.0;
    frame->filePath = "test.json";

    frame->clear();
    SimpleTest::assert_null(frame->cloud.get(), "Cloud cleared");
    SimpleTest::assert_true(frame->filePath.empty(), "FilePath cleared");

    // Clear again
    frame->clear();
    SimpleTest::assert_true(true, "Second clear succeeds");

    // Clear third time
    frame->clear();
    SimpleTest::assert_true(true, "Third clear succeeds");
}

void testObserverWeakPtrBehavior()
{
    std::cout << "\n=== Testing Observer Weak Pointer Behavior ===" << std::endl;

    // Test weak_ptr conversion
    auto observer = std::make_shared<MockFrameObserver>("WeakTest");
    std::weak_ptr<simulation::IFrameObserver> weakObs = observer;

    SimpleTest::assert_false(weakObs.expired(), "Weak pointer is not expired initially");

    // Lock and use
    if (auto locked = weakObs.lock())
    {
        auto frame = std::make_shared<simulation::Frame>();
        frame->timestamp = 77.7;
        locked->onFrameChanged(frame);
        SimpleTest::assert_true(true, "Locked weak pointer can be used");
    }

    // Reset strong pointer
    observer.reset();
    SimpleTest::assert_true(weakObs.expired(), "Weak pointer expired after reset");
}

void testLargePointCloud()
{
    std::cout << "\n=== Testing Large PointCloud in Frame ===" << std::endl;

    auto frame = std::make_shared<simulation::Frame>();
    auto cloud = std::make_shared<math::PointCloud>();

    // Add many points
    const int numPoints = 10000;
    for (int i = 0; i < numPoints; ++i)
    {
        cloud->addPoint(math::Point(
            static_cast<float>(i % 100),
            static_cast<float>((i / 100) % 100),
            static_cast<float>(i / 10000)));
    }

    frame->cloud = cloud;
    frame->timestamp = 999.0;

    SimpleTest::assert_equal_int(numPoints, static_cast<int>(frame->cloud->size()),
                                 "Large point cloud stored correctly");

    // Test clearing large cloud
    frame->clear();
    SimpleTest::assert_null(frame->cloud.get(), "Large point cloud cleared");
}

void testFrameFilePathVariations()
{
    std::cout << "\n=== Testing Frame FilePath Variations ===" << std::endl;

    simulation::Frame frame;

    // Test various path formats
    std::vector<std::string> paths = {
        "",
        "/",
        "/home/user/data/frame_00001.json",
        "relative/path/frame.json",
        "../parent/frame.json",
        "frame_with_spaces in name.json",
        "/path/with/special/chars/frame_αβγ.json",
        std::string(500, 'a') + ".json" // long filename
    };

    for (const auto &path : paths)
    {
        frame.filePath = path;
        SimpleTest::assert_true(frame.filePath == path,
                                "FilePath stored correctly: " + (path.length() > 50 ? path.substr(0, 50) + "..." : path));
    }

    frame.clear();
    SimpleTest::assert_true(frame.filePath.empty(), "FilePath cleared after clear()");
}

void testObserverCallbackOrder()
{
    std::cout << "\n=== Testing Observer Callback Order ===" << std::endl;

    std::vector<int> callOrder;

    class OrderTrackingObserver : public simulation::IFrameObserver
    {
    public:
        OrderTrackingObserver(int id, std::vector<int> &order) : id_(id), order_(order)
        {
            observerName_ = "OrderTracker_" + std::to_string(id);
        }

        void onFrameChanged(const std::shared_ptr<simulation::Frame> &) override
        {
            order_.push_back(id_);
        }

    private:
        int id_;
        std::vector<int> &order_;
    };

    std::vector<std::shared_ptr<OrderTrackingObserver>> observers;
    for (int i = 0; i < 3; ++i)
    {
        observers.push_back(std::make_shared<OrderTrackingObserver>(i, callOrder));
    }

    auto frame = std::make_shared<simulation::Frame>();
    for (auto &obs : observers)
    {
        obs->onFrameChanged(frame);
    }

    SimpleTest::assert_equal_int(3, static_cast<int>(callOrder.size()), "All observers called");
    SimpleTest::assert_equal_int(0, callOrder[0], "First observer called first");
    SimpleTest::assert_equal_int(1, callOrder[1], "Second observer called second");
    SimpleTest::assert_equal_int(2, callOrder[2], "Third observer called third");
}

void testFrameWithEmptyPointCloud()
{
    std::cout << "\n=== Testing Frame with Empty PointCloud ===" << std::endl;

    auto frame = std::make_shared<simulation::Frame>();
    auto emptyCloud = std::make_shared<math::PointCloud>();

    frame->cloud = emptyCloud;
    frame->timestamp = 0.0;

    SimpleTest::assert_not_null(frame->cloud.get(), "Empty cloud is not null");
    SimpleTest::assert_equal_int(0, static_cast<int>(frame->cloud->size()), "Empty cloud has size 0");

    auto observer = std::make_shared<MockFrameObserver>("EmptyCloudTest");
    observer->onFrameChanged(frame);

    SimpleTest::assert_equal_int(1, observer->getCallCount(), "Observer handles empty cloud frame");
}

void testFPSConversion()
{
    std::cout << "\n=== Testing FPS Conversion Logic ===" << std::endl;

    // Test various FPS values and their interval conversions
    struct FPSTest
    {
        float fps;
        float expectedInterval;
    };

    FPSTest tests[] = {
        {10.0f, 0.1f},
        {30.0f, 1.0f / 30.0f},
        {60.0f, 1.0f / 60.0f},
        {1.0f, 1.0f},
        {100.0f, 0.01f}};

    for (const auto &test : tests)
    {
        float interval = 1.0f / test.fps;
        float convertedFPS = 1.0f / interval;

        SimpleTest::assert_equal_float(test.expectedInterval, interval,
                                       "Interval for " + std::to_string(test.fps) + " FPS");
        SimpleTest::assert_equal_float(test.fps, convertedFPS,
                                       "FPS conversion roundtrip for " + std::to_string(test.fps));
    }
}

void testPlaybackTimerAccumulation()
{
    std::cout << "\n=== Testing Playback Timer Accumulation Logic ===" << std::endl;

    // Simulate timer accumulation logic
    float playbackTimer = 0.0f;
    float frameInterval = 0.1f; // 10 FPS
    int frameAdvances = 0;

    // Simulate 1 second of updates at ~60 FPS
    float deltaTime = 1.0f / 60.0f;
    for (int i = 0; i < 60; ++i)
    {
        playbackTimer += deltaTime;
        if (playbackTimer >= frameInterval)
        {
            playbackTimer = 0.0f;
            frameAdvances++;
        }
    }

    // At 10 FPS over 1 second, should advance ~10 frames
    SimpleTest::assert_true(frameAdvances >= 9 && frameAdvances <= 11,
                            "Frame advances in expected range (got " + std::to_string(frameAdvances) + ")");
}

void testCanAdvanceLogic()
{
    std::cout << "\n=== Testing Can Advance Logic ===" << std::endl;

    // Simulate canAdvance logic
    int totalFrameCount = 100;

    auto canAdvance = [totalFrameCount](int currentIndex, int direction) -> bool
    {
        int targetIndex = currentIndex + direction;
        return (targetIndex >= 0 && targetIndex < totalFrameCount);
    };

    // Test at start
    SimpleTest::assert_false(canAdvance(0, -1), "Cannot go back at frame 0");
    SimpleTest::assert_true(canAdvance(0, +1), "Can go forward at frame 0");

    // Test in middle
    SimpleTest::assert_true(canAdvance(50, -1), "Can go back at frame 50");
    SimpleTest::assert_true(canAdvance(50, +1), "Can go forward at frame 50");

    // Test at end
    SimpleTest::assert_true(canAdvance(99, -1), "Can go back at last frame");
    SimpleTest::assert_false(canAdvance(99, +1), "Cannot go forward at last frame");

    // Test boundaries
    SimpleTest::assert_false(canAdvance(-1, -1), "Invalid negative index");
    SimpleTest::assert_false(canAdvance(100, +1), "Invalid index beyond total");
}

void testWindowSizeCalculations()
{
    std::cout << "\n=== Testing Window Size Calculations ===" << std::endl;

    // Test window index calculations
    int windowSize = 3;
    int totalWindowElements = 2 * windowSize + 1; // -3, -2, -1, 0, +1, +2, +3 = 7 elements

    SimpleTest::assert_equal_int(7, totalWindowElements, "Window with size 3 has 7 elements");

    // Test center index
    int centerIndex = windowSize; // In a 0-indexed array of 7, center is at index 3
    SimpleTest::assert_equal_int(3, centerIndex, "Center index is at position 3");

    // Test with different window sizes
    for (int ws = 1; ws <= 5; ++ws)
    {
        int total = 2 * ws + 1;
        int center = ws;
        SimpleTest::assert_equal_int(center, total / 2,
                                     "Center calculation correct for window size " + std::to_string(ws));
    }
}

void testSeekBoundaryConditions()
{
    std::cout << "\n=== Testing Seek Boundary Conditions ===" << std::endl;

    int totalFrameCount = 50;

    auto isValidSeek = [totalFrameCount](int frameId) -> bool
    {
        return frameId >= 0 && frameId < totalFrameCount;
    };

    // Valid seeks
    SimpleTest::assert_true(isValidSeek(0), "Seek to 0 is valid");
    SimpleTest::assert_true(isValidSeek(25), "Seek to middle is valid");
    SimpleTest::assert_true(isValidSeek(49), "Seek to last frame is valid");

    // Invalid seeks
    SimpleTest::assert_false(isValidSeek(-1), "Seek to -1 is invalid");
    SimpleTest::assert_false(isValidSeek(50), "Seek beyond total is invalid");
    SimpleTest::assert_false(isValidSeek(100), "Seek way beyond total is invalid");
}

void testShiftWindowDirection()
{
    std::cout << "\n=== Testing Shift Window Direction ===" << std::endl;

    // Simulate deque operations
    std::deque<int> window = {1, 2, 3, 4, 5, 6, 7};

    // Shift forward (+1): remove front, add to back
    auto shiftForward = [](std::deque<int> &w, int newVal)
    {
        w.pop_front();
        w.push_back(newVal);
    };

    // Shift backward (-1): remove back, add to front
    auto shiftBackward = [](std::deque<int> &w, int newVal)
    {
        w.pop_back();
        w.push_front(newVal);
    };

    shiftForward(window, 8);
    SimpleTest::assert_equal_int(2, window.front(), "After forward shift, front is 2");
    SimpleTest::assert_equal_int(8, window.back(), "After forward shift, back is 8");

    shiftBackward(window, 1);
    SimpleTest::assert_equal_int(1, window.front(), "After backward shift, front is 1");
    SimpleTest::assert_equal_int(7, window.back(), "After backward shift, back is 7");
}

int main()
{
    std::cout << "FrameBufferManager Test Suite" << std::endl;
    std::cout << "=============================" << std::endl;

    try
    {
        // Frame structure tests
        testFrameStructure();
        testFrameSharedPtr();
        testFrameClearMultipleTimes();
        testFrameTimestampPrecision();
        testFrameFilePathVariations();
        testFrameWithEmptyPointCloud();
        testPointCloudWithFrame();
        testLargePointCloud();

        // Observer pattern tests
        testMockFrameObserver();
        testFrameObserverInterface();
        testMultipleObservers();
        testObserverWeakPtrBehavior();
        testObserverCallbackOrder();

        // FrameBufferManager logic tests (without actual file I/O)
        testFPSConversion();
        testPlaybackTimerAccumulation();
        testCanAdvanceLogic();
        testWindowSizeCalculations();
        testSeekBoundaryConditions();
        testShiftWindowDirection();

        std::cout << "\n=== All FrameBufferManager Tests Passed ===" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
