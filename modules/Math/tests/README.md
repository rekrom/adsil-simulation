# Math Module Tests

This directory contains comprehensive tests for the Math module of the ADSIL Analyzer project.

## Test Files

### Core Component Tests

- **`VectorTest.cpp`** - Tests for the `Vector` class

  - Construction and basic operations
  - Magnitude and normalization
  - Dot and cross products
  - Arithmetic operations (+, -, \*, scalar multiplication)
  - GLM library integration
  - String representation

- **`PointTest.cpp`** - Tests for the `Point` class

  - Construction and coordinate access
  - Distance calculations
  - Vector conversion operations
  - Point-Vector arithmetic
  - GLM library integration
  - Point-Point operations

- **`PointCloudTest.cpp`** - Tests for the `PointCloud` class
  - Construction with and without initial points
  - Adding single and multiple points
  - Cloud merging operations
  - Memory management and clearing
  - Large dataset handling

### Mathematical Utilities Tests

- **`ConstantsTest.cpp`** - Tests for mathematical constants

  - Zero, unity, and fraction constants
  - PI and trigonometric constants
  - Degree/radian conversion constants
  - Epsilon values for floating-point comparisons
  - Mathematical relationships validation

- **`RotationUtilsTest.cpp`** - Tests for rotation utilities

  - Degree/radian conversion functions
  - Roll-Pitch-Yaw rotation operations
  - Euler angle calculations
  - Rotation matrix generation
  - Magnitude preservation in rotations

- **`MathHelperTest.cpp`** - Tests for geometric helper functions
  - Line-plane intersection calculations
  - Point-in-convex-quad testing
  - Edge cases and parallel line handling
  - 3D geometric operations

### Integration Tests

- **`MathIntegrationTest.cpp`** - Integration tests between components
  - Point-Vector interoperability
  - Rotation and geometry integration
  - PointCloud transformations
  - Mathematical constants usage in real scenarios
  - GLM library integration consistency
  - Vector operation mathematical properties

## Building and Running Tests

### Build All Tests

```bash
cd /path/to/adsil_analyzer_cpp/build
cmake ..
make -j$(nproc)
```

### Run Individual Tests

```bash
cd build/bin
./test_VectorTest
./test_PointTest
./test_PointCloudTest
./test_ConstantsTest
./test_RotationUtilsTest
./test_MathHelperTest
./test_MathIntegrationTest
```

### Run All Tests with CTest

```bash
cd build
ctest -R "test_.*Test" --verbose
```

### Run Only Math Module Tests

```bash
cd build
ctest -R "test_(Constants|Vector|Point|PointCloud|MathHelper|RotationUtils|MathIntegration)Test"
```

## Test Coverage

The tests cover:

### Functionality Testing

- ✅ All public methods and operators
- ✅ Edge cases (zero vectors, empty point clouds, etc.)
- ✅ Error conditions (parallel lines, division by zero, etc.)
- ✅ Mathematical properties and invariants
- ✅ Performance with large datasets

### Integration Testing

- ✅ Inter-component compatibility
- ✅ GLM library integration
- ✅ Mathematical consistency across operations
- ✅ Real-world usage scenarios

### Robustness Testing

- ✅ Floating-point precision handling
- ✅ Memory management
- ✅ Thread safety (where applicable)
- ✅ Input validation

## Test Philosophy

These tests follow a **comprehensive validation** approach:

1. **Unit Tests**: Each component is tested in isolation
2. **Integration Tests**: Components are tested working together
3. **Property Tests**: Mathematical properties are validated
4. **Edge Case Tests**: Boundary conditions and error cases
5. **Performance Tests**: Large dataset handling

## Adding New Tests

When adding new functionality to the Math module:

1. Add unit tests for the new functionality
2. Update integration tests if the new feature interacts with existing components
3. Consider edge cases and error conditions
4. Validate mathematical properties
5. Test GLM integration if applicable

## Error Handling

Tests use `assert()` statements for validation and provide clear pass/fail messages. Failed assertions will:

- Print helpful error messages
- Show expected vs actual values
- Indicate which test case failed
- Terminate with non-zero exit code

## Debugging Failed Tests

If a test fails:

1. Check the assertion message for specific details
2. Verify mathematical expectations are correct
3. Consider floating-point precision issues (use `EPSILON_F` comparisons)
4. Run tests individually to isolate the problem
5. Use a debugger if needed: `gdb ./test_VectorTest`

## Performance Notes

- Tests include performance validation for large datasets
- All tests complete in under 1 second on typical hardware
- Memory usage is monitored for large point clouds
- No memory leaks should occur during test execution
