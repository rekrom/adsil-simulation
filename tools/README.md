# Unused Features Detection

This directory contains tools to detect unused features in the ADSIL Simulation codebase.

## Tools

### `detect_unused_features.py`

The main tool that analyzes the C++ modular codebase to detect:

1. **Unused modules** - Entire libraries that are compiled but not used
2. **Missing links** - Modules that are used but not linked in CMakeLists.txt
3. **Linking issues** - Modules that are linked but not actually used
4. **Dependency analysis** - Complete dependency graph of module relationships

### Usage

```bash
# Run the detection tool
python3 tools/detect_unused_features.py

# Or use the integration script
./tools/check_unused_features.sh
```

### Output

The tool generates:
- Console report with detailed analysis
- JSON report saved to `tools/unused_features_report.json`
- Exit code 0 if no issues, 1 if issues found

### Example Output

```
📊 MODULE USAGE ANALYSIS
   Total modules: 9
   Linked in CMake: 9
   Actually used: 9
   Linked but unused: 0
   Used but not linked: 0
   Completely unused: 0

✅ All modules are properly configured!
```

## Integration

### With CI/CD

Add to your CI pipeline:

```yaml
- name: Check for unused features
  run: ./tools/check_unused_features.sh
```

### With CMake

You can add a custom target to run the check:

```cmake
add_custom_target(check-unused
    COMMAND ${CMAKE_SOURCE_DIR}/tools/check_unused_features.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking for unused features"
)
```

## How It Works

1. **Module Discovery**: Scans the `modules/` directory for available modules
2. **CMake Analysis**: Parses `apps/adsil_analyzer/CMakeLists.txt` to find linked modules
3. **Include Analysis**: Traces `#include` statements to find actually used modules
4. **Dependency Graph**: Builds transitive dependency relationships
5. **Report Generation**: Compares linked vs used modules to find issues

## Common Issues Found

- **Missing Links**: Modules used through dependencies but not explicitly linked
- **Over-linking**: Modules linked but not actually used (increases binary size)
- **Dead Code**: Entire modules that can be removed from the build

## Benefits

- **Faster Builds**: Remove unused modules from compilation
- **Smaller Binaries**: Reduce linking overhead
- **Cleaner Code**: Identify and remove dead code
- **Better Dependencies**: Ensure proper module linking