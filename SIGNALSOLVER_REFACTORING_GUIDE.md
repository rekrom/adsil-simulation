# SignalSolver Refactoring Guide

## Overview

This guide will help you manually refactor the `SignalSolver::solve()` method to reduce cognitive complexity and improve code maintainability. The current method has a cognitive complexity of 30+ (threshold is 25) and needs to be broken down into smaller, focused functions.

## Current Issues

- **Cognitive Complexity**: Single method handling multiple responsibilities
- **Magic Numbers**: Hardcoded values like `2.0F`, `0.0f`
- **Code Style**: Mixed floating point suffixes (`0.0f` vs `0.0F`)
- **Long Method**: 200+ lines doing too many things

## Refactoring Strategy

### Step 1: Add Required Includes

**File: `SignalSolver.cpp`**

Add these includes at the top (after existing includes):

```cpp
#include <math/Vector.hpp>
#include <math/Constants.hpp>
#include <limits>
#include <cmath>

using math::Point;
using math::Vector;
using namespace math::constants;  // Access to ZERO_F, HALF_DIVISOR, etc.
```

**Note**: The math constants are now available project-wide in the `math::constants` namespace. No need for local constants anymore!

### Step 2: Add Helper Method Declarations

**File: `SignalSolver.hpp`**

Add these private method declarations:

```cpp
private:
    // Helper methods for solve()
    std::vector<std::vector<float>> calculateToFValues(
        const SharedVec<Device>& txs,
        const SharedVec<Device>& rxs,
        const std::shared_ptr<math::PointCloud>& allPoints,
        std::shared_ptr<math::PointCloud>& result);

    std::shared_ptr<math::PointCloud> performAdsilTriangulation(
        const std::vector<std::vector<float>>& toFVals,
        const SharedVec<Device>& txs,
        const SharedVec<Device>& rxs);

    std::vector<math::Point> calculateFovCorners(
        const std::shared_ptr<Device>& tx);

    math::Point findClosestPointInFov(
        const std::shared_ptr<Device>& tx,
        const std::shared_ptr<Device>& rx,
        const std::shared_ptr<math::PointCloud>& allPoints);
```

### Step 3: Refactor the Main solve() Method

**Current Structure Analysis:**

```
solve() method currently does:
├── Setup (lines 12-21)
├── FOV Corner Calculation (lines 26-52) → Extract to calculateFovCorners()
├── ToF Calculation Loop (lines 54-90) → Extract to calculateToFValues()
├── ADSIL Triangulation (lines 98-185) → Extract to performAdsilTriangulation()
└── Return result
```

**New solve() method should look like:**

```cpp
std::shared_ptr<math::PointCloud> SignalSolver::solve()
{
    auto result = std::make_shared<math::PointCloud>();
    auto allPoints = scene_->getMergedPointCloud();

    const auto& txs = scene_->getTransmitters();
    const auto& rxs = scene_->getReceivers();

    // Calculate Time-of-Flight values and FOV corners
    auto toFVals = calculateToFValues(txs, rxs, allPoints, result);

    // Perform ADSIL triangulation if we have valid ToF data
    if (!result->empty()) {
        auto adsilResult = performAdsilTriangulation(toFVals, txs, rxs);
        return adsilResult;
    }

    return result;
}
```

### Step 4: Implement calculateFovCorners() Helper

**Purpose**: Extract FOV corner calculation logic

**Extract from lines 26-52**:

```cpp
std::vector<math::Point> SignalSolver::calculateFovCorners(const std::shared_ptr<Device>& tx)
{
    float fovH = tx->getHorizontalFovRad();
    float fovV = tx->getVerticalFovRad();
    float range = tx->getRange();

    float halfW = range * tanf(fovH / HALF_DIVISOR);
    float halfH = range * tanf(fovV / HALF_DIVISOR);

    auto newPoint1 = std::make_shared<spatial::TransformNode>();
    auto newPoint2 = std::make_shared<spatial::TransformNode>();
    auto newPoint3 = std::make_shared<spatial::TransformNode>();
    auto newPoint4 = std::make_shared<spatial::TransformNode>();

    tx->getTransformNode()->addChild(newPoint1);
    tx->getTransformNode()->addChild(newPoint2);
    tx->getTransformNode()->addChild(newPoint3);
    tx->getTransformNode()->addChild(newPoint4);

    // Set local transforms for each FOV corner
    newPoint1->setLocalTransform(spatial::Transform({-halfW, halfH, range}, {ZERO_F, ZERO_F, ZERO_F}));
    newPoint2->setLocalTransform(spatial::Transform({halfW, halfH, range}, {ZERO_F, ZERO_F, ZERO_F}));
    newPoint3->setLocalTransform(spatial::Transform({halfW, -halfH, range}, {ZERO_F, ZERO_F, ZERO_F}));
    newPoint4->setLocalTransform(spatial::Transform({-halfW, -halfH, range}, {ZERO_F, ZERO_F, ZERO_F}));

    // Get global positions
    std::vector<math::Point> corners;
    corners.push_back(newPoint1->getGlobalTransform().getPosition());
    corners.push_back(newPoint2->getGlobalTransform().getPosition());
    corners.push_back(newPoint3->getGlobalTransform().getPosition());
    corners.push_back(newPoint4->getGlobalTransform().getPosition());

    return corners;
}
```

### Step 5: Implement findClosestPointInFov() Helper

**Purpose**: Extract closest point finding logic

**Extract from lines 66-88**:

```cpp
math::Point SignalSolver::findClosestPointInFov(
    const std::shared_ptr<Device>& tx,
    const std::shared_ptr<Device>& rx,
    const std::shared_ptr<math::PointCloud>& allPoints)
{
    if (!allPoints || allPoints->empty()) {
        return math::Point(ZERO_F, ZERO_F, ZERO_F); // Return invalid point
    }

    // Filter points inside both FOVs
    auto inTxFov = tx->pointsInFov(*allPoints);
    if (!inTxFov || inTxFov->size() == 0) {
        return math::Point(ZERO_F, ZERO_F, ZERO_F); // Return invalid point
    }

    auto inRxFov = rx->pointsInFov(*inTxFov);
    if (!inRxFov || inRxFov->size() == 0) {
        return math::Point(ZERO_F, ZERO_F, ZERO_F); // Return invalid point
    }

    // Find the closest point (based on Tx + Rx distance)
    float minDist = std::numeric_limits<float>::max();
    math::Point closest(ZERO_F, ZERO_F, ZERO_F);

    for (const auto& pt : inRxFov->getPoints()) {
        float dTx = pt.distanceTo(tx->getTransformNode()->getGlobalTransform().getPosition());
        float dRx = pt.distanceTo(rx->getTransformNode()->getGlobalTransform().getPosition());
        float total = dTx + dRx;

        if (total < minDist) {
            minDist = total;
            closest = pt;
        }
    }

    return closest;
}
```

### Step 6: Implement calculateToFValues() Helper

**Purpose**: Extract ToF calculation loop

```cpp
std::vector<std::vector<float>> SignalSolver::calculateToFValues(
    const SharedVec<Device>& txs,
    const SharedVec<Device>& rxs,
    const std::shared_ptr<math::PointCloud>& allPoints,
    std::shared_ptr<math::PointCloud>& result)
{
    size_t tx_size = txs.size();
    size_t rx_size = rxs.size();

    std::vector<std::vector<float>> toFVals(tx_size, std::vector<float>(rx_size, ZERO_F));

    int tx_no = -1;
    for (const auto& tx : txs) {
        tx_no++;

        // Calculate and add FOV corners to result
        auto fovCorners = calculateFovCorners(tx);
        for (const auto& corner : fovCorners) {
            result->addPoint(corner);
        }

        int rx_no = -1;
        for (const auto& rx : rxs) {
            rx_no++;

            math::Point closest = findClosestPointInFov(tx, rx, allPoints);

            // Check if we found a valid point
            if (closest.x() != ZERO_F || closest.y() != ZERO_F || closest.z() != ZERO_F) {
                float dTx = closest.distanceTo(tx->getTransformNode()->getGlobalTransform().getPosition());
                float dRx = closest.distanceTo(rx->getTransformNode()->getGlobalTransform().getPosition());
                float total = dTx + dRx;

                toFVals[tx_no][rx_no] = total;
                solveCount_++;
                result->addPoint(closest);
            }
        }
    }

    return toFVals;
}
```

### Step 7: Implement performAdsilTriangulation() Helper

**Purpose**: Extract ADSIL triangulation algorithm

**Extract from lines 98-185** (replace magic numbers):

```cpp
std::shared_ptr<math::PointCloud> SignalSolver::performAdsilTriangulation(
    const std::vector<std::vector<float>>& toFVals,
    const SharedVec<Device>& txs,
    const SharedVec<Device>& rxs)
{
    auto adsilResult = std::make_shared<math::PointCloud>();

    size_t rx_size = rxs.size();
    if (rx_size != 4) {
        throw std::runtime_error("rx_size must be 4");
    }

    int tx_no = -1;
    for (const auto& tx : txs) {
        tx_no++;

        // Check if ToF values are valid (replace 0.0f with ZERO_F)
        if (toFVals[tx_no][0] <= ZERO_F || toFVals[tx_no][1] <= ZERO_F ||
            toFVals[tx_no][2] <= ZERO_F || toFVals[tx_no][3] <= ZERO_F) {
            continue;
        }

        // ADSIL algorithm implementation
        float R0 = toFVals[tx_no][0] / HALF_DIVISOR;
        float R1 = toFVals[tx_no][1] - R0;
        float R2 = toFVals[tx_no][2] - R0;
        float R3 = toFVals[tx_no][3] - R0;

        auto c1 = rxs[1]->getGlobalTransform().getPosition();
        auto c2 = rxs[2]->getGlobalTransform().getPosition();
        auto c3 = rxs[3]->getGlobalTransform().getPosition();

        Vector P1P2 = c2.toVectorFrom(c1);
        float d = std::sqrt(P1P2.dot(P1P2));
        if (d == ZERO_F) {
            continue;
        }

        Vector ex = P1P2 * (1.0F / d);
        Vector c1c3 = c3.toVectorFrom(c1);
        float i = ex.dot(c1c3);

        Vector temp = c1c3 - (ex * i);
        float tempLengthSquared = temp.dot(temp);
        if (tempLengthSquared == ZERO_F) {
            continue;
        }
        Vector ey = temp.normalized();
        Vector ez = ex.cross(ey);
        float j = ey.dot(c1c3);

        // Replace magic numbers with HALF_DIVISOR
        float x = (R1 * R1 - R2 * R2 + d * d) / (HALF_DIVISOR * d);
        float y_numerator = R1 * R1 - R3 * R3 + i * i + j * j - HALF_DIVISOR * i * x;
        float y = y_numerator / (HALF_DIVISOR * j);

        float zSquared = R1 * R1 - x * x - y * y;
        if (zSquared < ZERO_F) {
            continue;
        }

        float z1 = std::sqrt(zSquared);
        float z2 = -z1;

        Vector resultVec = (ex * x) + (ey * y) + (ez * z1);
        Point result_point1 = c1 + resultVec;
        resultVec = (ex * x) + (ey * y) + (ez * z2);
        Point result_point2 = c1 + resultVec;

        auto possible_points = std::make_shared<math::PointCloud>();
        possible_points->addPoint(result_point1);
        possible_points->addPoint(result_point2);

        auto inFovSolution = tx->pointsInFov(*possible_points);
        adsilResult->addPoints(inFovSolution->getPoints());
    }

    return adsilResult;
}
```

### Step 8: Fix Floating Point Suffixes

Replace all instances of:

- `0.0f` → `ZERO_F` or `0.0F`
- `2.0f` → `HALF_DIVISOR`
- `1.0f` → `1.0F`

### Step 9: Testing Checklist

After implementing all changes:

1. **Build Test**: `cmake --build build --target adsil_analyzer -j4`
2. **Static Analysis**: Run clang-tidy to verify complexity reduction
3. **Functionality Test**: Ensure the algorithm still produces correct results
4. **Performance Test**: Verify no significant performance regression

### Step 10: Verification Commands

```bash
# Build the project
cd /home/rkrm-dev/Desktop/adsil_analyzer_cpp
cmake --build build --target adsil_analyzer -j4

# Check cognitive complexity (should be under 25 now)
clang-tidy modules/Simulation/src/SignalSolver.cpp \
  --checks='-*,readability-function-cognitive-complexity' \
  --header-filter=modules/Simulation \
  -- -Imodules/Simulation/include -Imodules/Core/include \
     -Imodules/Math/include -Imodules/Spatial/include \
     -Imodules/Geometry/include -std=c++17
```

## Expected Results

After refactoring:

- ✅ **Cognitive Complexity**: Reduced from 30+ to under 25
- ✅ **Code Readability**: Clear separation of concerns
- ✅ **Maintainability**: Smaller, focused functions
- ✅ **Testing**: Each helper method can be unit tested independently
- ✅ **Magic Numbers**: Replaced with named constants from `math::constants`
- ✅ **Project-wide Constants**: Math constants available across all modules

## Additional Benefits of Math Constants Module

The new `math::constants` module provides:

1. **Consistency**: Same constants used throughout the project
2. **Maintainability**: Change constants in one place
3. **Readability**: Self-documenting constant names
4. **Performance**: `constexpr` values computed at compile time
5. **Type Safety**: Separate float/double versions prevent implicit conversions

## Common Pitfalls to Avoid

1. **Include Dependencies**: Make sure all required headers are included
2. **Namespace Issues**: Use proper `math::Point` and `math::Vector` qualifications
3. **Const Correctness**: Maintain const-correctness in method signatures
4. **Error Handling**: Don't lose existing error handling logic
5. **Performance**: Avoid unnecessary copies of large objects

## Final File Structure

```
SignalSolver.cpp
├── Includes & Constants
├── Constructor (unchanged)
├── solve() - Main orchestrator (simplified)
├── calculateToFValues() - ToF calculation logic
├── performAdsilTriangulation() - ADSIL algorithm
├── calculateFovCorners() - FOV geometry
└── findClosestPointInFov() - Point filtering
```

This refactoring transforms a complex 200-line method into a clean, maintainable design with clear separation of concerns.

## Available Math Constants

The new `math::constants` namespace provides the following constants for project-wide use:

### Basic Constants

- `ZERO_F` / `ZERO_D` - Zero values (float/double)
- `ONE_F` / `ONE_D` - Unity values (float/double)
- `HALF_DIVISOR` / `HALF_DIVISOR_D` - Division by 2 (float/double)

### Fractions

- `QUARTER` (0.25F)
- `HALF` (0.5F)
- `THREE_QUARTERS` (0.75F)

### Tolerance Values

- `EPSILON_F` / `EPSILON_D` - For floating point comparisons

### Mathematical Constants

- `PI_F` / `PI_D` - Pi values (float/double)
- `TWO_PI_F` / `TWO_PI_D` - 2π values
- `HALF_PI_F` / `HALF_PI_D` - π/2 values

### Conversion Constants

- `DEG_TO_RAD_F` / `DEG_TO_RAD_D` - Degree to radian conversion
- `RAD_TO_DEG_F` / `RAD_TO_DEG_D` - Radian to degree conversion

### Usage Example

```cpp
#include <math/Constants.hpp>
using namespace math::constants;

// Instead of hardcoding 2.0F
float halfAngle = angle / HALF_DIVISOR;

// Instead of hardcoding 0.0F
if (distance > ZERO_F) { /* ... */ }

// For comparisons
if (std::abs(value1 - value2) < EPSILON_F) { /* values are equal */ }
```
