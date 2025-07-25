#!/usr/bin/env python3
"""
ADSIL Simulation - Unused Feature Detection Tool

This tool analyzes the C++ codebase to detect:
1. Unused modules (entire libraries not used by main application)
2. Unused functions/classes within used modules
3. Unused external dependencies
4. Unused CMake targets

Usage: python3 tools/detect_unused_features.py
"""

import os
import re
import json
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple
from collections import defaultdict, deque


class UsageAnalyzer:
    """Analyzes C++ code for unused features"""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.modules_dir = self.project_root / "modules"
        self.apps_dir = self.project_root / "apps"
        self.build_dir = self.project_root / "build"
        
        # Store analysis results
        self.module_dependencies: Dict[str, Set[str]] = defaultdict(set)
        self.include_graph: Dict[str, Set[str]] = defaultdict(set)
        self.function_definitions: Dict[str, Set[str]] = defaultdict(set)
        self.function_calls: Dict[str, Set[str]] = defaultdict(set)
        self.cmake_targets: Dict[str, Set[str]] = defaultdict(set)
        
    def find_cpp_files(self, directory: Path) -> List[Path]:
        """Find all C++ source and header files"""
        cpp_extensions = {'.cpp', '.hpp', '.h', '.c', '.cc'}
        cpp_files = []
        
        for ext in cpp_extensions:
            cpp_files.extend(directory.rglob(f"*{ext}"))
            
        return cpp_files
    
    def parse_includes(self, file_path: Path) -> List[str]:
        """Extract all #include statements from a file"""
        includes = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                for line in f:
                    # Match #include "..." and #include <...>
                    match = re.match(r'^\s*#include\s*[<"]([^>"]+)[>"]', line)
                    if match:
                        includes.append(match.group(1))
        except Exception as e:
            print(f"Warning: Could not read {file_path}: {e}")
        
        return includes
    
    def parse_cmake_targets(self, cmake_file: Path) -> Set[str]:
        """Extract CMake targets from CMakeLists.txt"""
        targets = set()
        try:
            with open(cmake_file, 'r', encoding='utf-8') as f:
                content = f.read()
                
                # Find add_library and add_executable calls
                library_matches = re.findall(r'add_library\s*\(\s*(\w+)', content, re.IGNORECASE)
                executable_matches = re.findall(r'add_executable\s*\(\s*(\w+)', content, re.IGNORECASE)
                
                targets.update(library_matches)
                targets.update(executable_matches)
                
        except Exception as e:
            print(f"Warning: Could not read {cmake_file}: {e}")
        
        return targets
    
    def analyze_dependencies(self):
        """Analyze module dependencies based on includes"""
        print("🔍 Analyzing module dependencies...")
        
        # Get all modules
        modules = [d.name for d in self.modules_dir.iterdir() if d.is_dir()]
        print(f"Found modules: {modules}")
        
        # Analyze each module's dependencies
        for module in modules:
            module_path = self.modules_dir / module
            cpp_files = self.find_cpp_files(module_path)
            
            for cpp_file in cpp_files:
                includes = self.parse_includes(cpp_file)
                for include in includes:
                    # Check if include is from another module
                    for other_module in modules:
                        if include.startswith(f"{other_module.lower()}/"):
                            self.module_dependencies[module].add(other_module)
                            self.include_graph[str(cpp_file.relative_to(self.project_root))].add(include)
    
    def analyze_actual_usage(self) -> Tuple[Set[str], Set[str]]:
        """Analyze which modules are actually used vs just linked"""
        print("🎯 Analyzing actual usage...")
        
        # Get modules that are explicitly linked in CMake
        linked_modules = self.analyze_cmake_dependencies()
        
        # Now trace through include dependencies to find really used modules
        used_modules = set()
        to_process = deque()
        
        # Start with main app files (both .cpp and .hpp)
        app_files = self.find_cpp_files(self.apps_dir)
        
        # Find modules directly included by the app
        for app_file in app_files:
            includes = self.parse_includes(app_file)
            
            for include in includes:
                module_name = include.split('/')[0]
                
                # Check for exact match first
                if (self.modules_dir / module_name).exists():
                    used_modules.add(module_name)
                    to_process.append(module_name)
                else:
                    # Check for case-insensitive match
                    all_modules = {d.name for d in self.modules_dir.iterdir() if d.is_dir()}
                    for actual_module in all_modules:
                        if actual_module.lower() == module_name.lower():
                            used_modules.add(actual_module)
                            to_process.append(actual_module)
                            break
        
        # BFS to find transitive dependencies through actual includes
        while to_process:
            current_module = to_process.popleft()
            for dep_module in self.module_dependencies.get(current_module, set()):
                if dep_module not in used_modules:
                    used_modules.add(dep_module)
                    to_process.append(dep_module)
        
        return linked_modules, used_modules
    
    def analyze_cmake_dependencies(self) -> Set[str]:
        """Analyze CMake target dependencies and return linked modules"""
        print("📦 Analyzing CMake targets...")
        
        # Parse the main app's CMakeLists.txt to see what's actually linked
        app_cmake = self.apps_dir / "adsil_analyzer" / "CMakeLists.txt"
        linked_modules = set()
        
        if app_cmake.exists():
            try:
                with open(app_cmake, 'r') as f:
                    content = f.read()
                
                # Find target_link_libraries section - handle multiline
                link_match = re.search(
                    r'target_link_libraries\s*\(\s*\$\{PROJECT_NAME\}(.*?)\)',
                    content, 
                    re.DOTALL | re.IGNORECASE
                )
                
                if link_match:
                    libs_section = link_match.group(1)
                    
                    # Remove comments from the section
                    lines = libs_section.split('\n')
                    filtered_lines = []
                    for line in lines:
                        # Remove everything after # (comments)
                        comment_pos = line.find('#')
                        if comment_pos != -1:
                            line = line[:comment_pos]
                        filtered_lines.append(line)
                    libs_section = '\n'.join(filtered_lines)
                    
                    # Extract individual library names, skipping keywords and external libs
                    all_words = re.findall(r'\b([A-Za-z]\w+)\b', libs_section)
                    
                    # Filter out CMake keywords and external libraries
                    cmake_keywords = {'PRIVATE', 'PUBLIC', 'INTERFACE'}
                    external_libs = {'glfw', 'OpenGL', 'GL', 'glm'}
                    
                    # Get actual module names
                    all_modules = {d.name for d in self.modules_dir.iterdir() if d.is_dir()}
                    
                    for word in all_words:
                        if word not in cmake_keywords and word not in external_libs:
                            if word in all_modules:
                                linked_modules.add(word)
                            elif word.lower() in {m.lower() for m in all_modules}:
                                # Case-insensitive match
                                for module in all_modules:
                                    if module.lower() == word.lower():
                                        linked_modules.add(module)
                                        break
                    
            except Exception as e:
                print(f"Warning: Could not parse {app_cmake}: {e}")
        
        return linked_modules
    
    def find_function_definitions(self, cpp_files: List[Path]) -> Dict[str, Set[str]]:
        """Find function definitions in C++ files"""
        function_defs = defaultdict(set)
        
        # Simple regex for function definitions (not perfect but good enough)
        function_pattern = re.compile(
            r'^\s*(?:(?:inline|static|virtual|constexpr|const|noexcept)\s+)*'
            r'(?:[\w:]+\s+)*'  # return type
            r'([\w:]+)\s*\('   # function name
            r'[^)]*\)\s*'      # parameters
            r'(?:const\s*)?'   # const qualifier
            r'(?:noexcept\s*)?'  # noexcept
            r'(?:override\s*)?'  # override
            r'[{;]',           # opening brace or semicolon
            re.MULTILINE
        )
        
        for cpp_file in cpp_files:
            try:
                with open(cpp_file, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    
                matches = function_pattern.findall(content)
                for match in matches:
                    # Filter out constructors/destructors and operators
                    if not re.match(r'^(~?\w+|operator.*)$', match):
                        function_defs[str(cpp_file.relative_to(self.project_root))].add(match)
                        
            except Exception as e:
                print(f"Warning: Could not analyze {cpp_file}: {e}")
        
        return function_defs
    
    def generate_report(self):
        """Generate a comprehensive unused features report"""
        print("\n" + "="*60)
        print("🔍 UNUSED FEATURES DETECTION REPORT")
        print("="*60)
        
        # Analyze dependencies
        self.analyze_dependencies()
        
        # Get all modules
        all_modules = {d.name for d in self.modules_dir.iterdir() if d.is_dir()}
        linked_modules, used_modules = self.analyze_actual_usage()
        
        # Calculate different categories
        unused_linked = linked_modules - used_modules  # Linked but not used
        unlinked_modules = all_modules - linked_modules  # Not even linked
        missing_linked = used_modules - linked_modules  # Used but not linked
        truly_unused = unused_linked | (unlinked_modules - used_modules)
        
        # Report analysis
        print(f"\n📊 MODULE USAGE ANALYSIS")
        print(f"   Total modules: {len(all_modules)}")
        print(f"   Linked in CMake: {len(linked_modules)}")
        print(f"   Actually used: {len(used_modules)}")
        print(f"   Linked but unused: {len(unused_linked)}")
        print(f"   Used but not linked: {len(missing_linked)}")
        print(f"   Completely unused: {len(truly_unused)}")
        
        # Report missing links (high priority issue)
        if missing_linked:
            print(f"\n⚠️  USED BUT NOT LINKED (should be added to CMakeLists.txt):")
            for module in sorted(missing_linked):
                print(f"   - {module}")
        
        # Report linked modules
        if linked_modules:
            print(f"\n🔗 LINKED MODULES (in CMakeLists.txt):")
            for module in sorted(linked_modules):
                status = "✅ USED" if module in used_modules else "❌ UNUSED"
                print(f"   - {module:15s} {status}")
        
        # Report actually used modules with their dependencies
        if used_modules:
            print(f"\n✅ ACTUALLY USED MODULES:")
            for module in sorted(used_modules):
                deps = self.module_dependencies.get(module, set()) & used_modules
                deps_str = ", ".join(sorted(deps)) if deps else "none"
                link_status = "LINKED" if module in linked_modules else "NOT LINKED"
                print(f"   - {module:15s} ({link_status}) (deps: {deps_str})")
        
        # Report unused modules
        if truly_unused:
            print(f"\n❌ COMPLETELY UNUSED MODULES:")
            for module in sorted(truly_unused):
                module_path = self.modules_dir / module
                cpp_files = self.find_cpp_files(module_path)
                total_lines = 0
                for cpp_file in cpp_files:
                    try:
                        with open(cpp_file, 'r') as f:
                            total_lines += len(f.readlines())
                    except:
                        pass
                
                category = "WAS LINKED" if module in unused_linked else "NOT LINKED"
                print(f"   - {module:15s} ({len(cpp_files):3d} files, ~{total_lines:4d} lines) [{category}]")
        
        # Report dependency graph for used modules only
        print(f"\n🔗 USED MODULE DEPENDENCY GRAPH:")
        for module in sorted(used_modules):
            deps = self.module_dependencies.get(module, set()) & used_modules
            if deps:
                print(f"   {module} -> {', '.join(sorted(deps))}")
        
        # Recommendations
        print(f"\n💡 RECOMMENDATIONS:")
        
        if missing_linked:
            print(f"   🔥 HIGH PRIORITY - Add to target_link_libraries:")
            for module in sorted(missing_linked):
                print(f"      + {module}")
        
        if unused_linked:
            print(f"   🧹 CLEANUP - Remove from target_link_libraries:")
            for module in sorted(unused_linked):
                print(f"      - {module}")
        
        if truly_unused:
            print(f"   🗑️  REMOVAL - Consider removing unused modules:")
            for module in sorted(truly_unused):
                print(f"      - {module}")
            print(f"      Or add feature flags to conditionally compile them")
        
        if not missing_linked and not unused_linked and not truly_unused:
            print(f"   ✅ All modules are properly configured!")
        
        # Summary
        print(f"\n📋 SUMMARY:")
        total_cpp_files = len(self.find_cpp_files(self.project_root))
        unused_files = 0
        if truly_unused:
            for module in truly_unused:
                unused_files += len(self.find_cpp_files(self.modules_dir / module))
        
        print(f"   Total C++ files: {total_cpp_files}")
        print(f"   Files in unused modules: {unused_files}")
        print(f"   Configuration issues: {len(missing_linked) + len(unused_linked)} modules")
        
        return {
            'all_modules': list(all_modules),
            'linked_modules': list(linked_modules),
            'used_modules': list(used_modules),
            'unused_linked': list(unused_linked),
            'missing_linked': list(missing_linked),
            'truly_unused': list(truly_unused),
            'module_dependencies': {k: list(v) for k, v in self.module_dependencies.items()},
            'total_files': total_cpp_files,
            'unused_files': unused_files,
            'has_issues': bool(missing_linked or unused_linked or truly_unused)
        }


def main():
    """Main entry point"""
    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    
    print("🚀 ADSIL Simulation - Unused Feature Detection Tool")
    print(f"📁 Project root: {project_root}")
    
    analyzer = UsageAnalyzer(project_root)
    
    try:
        results = analyzer.generate_report()
        
        # Save results to JSON
        output_file = Path(project_root) / "tools" / "unused_features_report.json"
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\n💾 Report saved to: {output_file}")
        
        # Return exit code based on findings
        return 1 if results['has_issues'] else 0
        
    except Exception as e:
        print(f"❌ Error during analysis: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())