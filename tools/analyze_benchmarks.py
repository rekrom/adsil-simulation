#!/usr/bin/env python3
"""
Build Performance Analysis Tool

This script analyzes benchmark results and generates performance trend reports.
"""

import json
import os
import glob
import argparse
import statistics
from datetime import datetime
from typing import List, Dict, Any

def load_benchmark_results(directory: str = ".") -> List[Dict[str, Any]]:
    """Load all benchmark result JSON files from a directory."""
    pattern = os.path.join(directory, "benchmark_results_*.json")
    results = []
    
    for file_path in glob.glob(pattern):
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
                data['file_path'] = file_path
                results.append(data)
        except (json.JSONDecodeError, FileNotFoundError) as e:
            print(f"Warning: Could not load {file_path}: {e}")
    
    # Sort by timestamp
    results.sort(key=lambda x: x.get('timestamp', ''))
    return results

def format_duration(seconds: int) -> str:
    """Format duration in seconds to human readable format."""
    if seconds < 60:
        return f"{seconds}s"
    elif seconds < 3600:
        minutes = seconds // 60
        remaining_seconds = seconds % 60
        return f"{minutes}m {remaining_seconds:02d}s"
    else:
        hours = seconds // 3600
        minutes = (seconds % 3600) // 60
        remaining_seconds = seconds % 60
        return f"{hours}h {minutes:02d}m {remaining_seconds:02d}s"

def calculate_statistics(values: List[float]) -> Dict[str, float]:
    """Calculate basic statistics for a list of values."""
    if not values:
        return {}
    
    return {
        'min': min(values),
        'max': max(values),
        'mean': statistics.mean(values),
        'median': statistics.median(values),
        'std_dev': statistics.stdev(values) if len(values) > 1 else 0.0
    }

def analyze_performance_trend(results: List[Dict[str, Any]]) -> None:
    """Analyze performance trends from benchmark results."""
    if not results:
        print("No benchmark results found.")
        return
    
    print("📊 Build Performance Analysis")
    print("=" * 50)
    print(f"Total benchmarks analyzed: {len(results)}")
    print()
    
    # Separate results by ccache usage
    ccache_results = [r for r in results if r.get('use_ccache', False)]
    no_ccache_results = [r for r in results if not r.get('use_ccache', False)]
    
    print("🔧 Configuration Summary:")
    print(f"  With ccache: {len(ccache_results)} runs")
    print(f"  Without ccache: {len(no_ccache_results)} runs")
    print()
    
    # Analyze by category
    categories = ['configure_seconds', 'build_seconds', 'test_seconds', 'total_seconds']
    category_names = ['Configure', 'Build', 'Test', 'Total']
    
    for category, name in zip(categories, category_names):
        print(f"📈 {name} Times:")
        
        if ccache_results:
            ccache_times = [r['timing'][category] for r in ccache_results if 'timing' in r and category in r['timing']]
            if ccache_times:
                stats = calculate_statistics(ccache_times)
                print(f"  With ccache:")
                print(f"    Mean: {format_duration(int(stats['mean']))}")
                print(f"    Median: {format_duration(int(stats['median']))}")
                print(f"    Range: {format_duration(int(stats['min']))} - {format_duration(int(stats['max']))}")
                if stats['std_dev'] > 0:
                    print(f"    Std Dev: {stats['std_dev']:.1f}s")
        
        if no_ccache_results:
            no_ccache_times = [r['timing'][category] for r in no_ccache_results if 'timing' in r and category in r['timing']]
            if no_ccache_times:
                stats = calculate_statistics(no_ccache_times)
                print(f"  Without ccache:")
                print(f"    Mean: {format_duration(int(stats['mean']))}")
                print(f"    Median: {format_duration(int(stats['median']))}")
                print(f"    Range: {format_duration(int(stats['min']))} - {format_duration(int(stats['max']))}")
                if stats['std_dev'] > 0:
                    print(f"    Std Dev: {stats['std_dev']:.1f}s")
        
        # Compare if both exist
        if ccache_results and no_ccache_results:
            ccache_times = [r['timing'][category] for r in ccache_results if 'timing' in r and category in r['timing']]
            no_ccache_times = [r['timing'][category] for r in no_ccache_results if 'timing' in r and category in r['timing']]
            
            if ccache_times and no_ccache_times:
                ccache_mean = statistics.mean(ccache_times)
                no_ccache_mean = statistics.mean(no_ccache_times)
                improvement = ((no_ccache_mean - ccache_mean) / no_ccache_mean) * 100
                
                if improvement > 0:
                    print(f"  🚀 ccache improvement: {improvement:.1f}% faster")
                else:
                    print(f"  ⚠️  ccache overhead: {abs(improvement):.1f}% slower")
        
        print()

def generate_recent_trend(results: List[Dict[str, Any]], count: int = 5) -> None:
    """Show trend for the most recent benchmark runs."""
    if len(results) < 2:
        return
    
    recent_results = results[-count:]
    print(f"📅 Recent Trend (last {len(recent_results)} runs):")
    print()
    
    print("| Date & Time | Build Type | ccache | Configure | Build | Test | Total |")
    print("|-------------|------------|--------|-----------|-------|------|-------|")
    
    for result in recent_results:
        timestamp = result.get('timestamp', 'Unknown')
        try:
            dt = datetime.fromisoformat(timestamp.replace('Z', '+00:00'))
            date_str = dt.strftime('%m/%d %H:%M')
        except:
            date_str = timestamp[:16] if len(timestamp) > 16 else timestamp
        
        build_type = result.get('build_type', 'Unknown')
        ccache = '✅' if result.get('use_ccache', False) else '❌'
        
        timing = result.get('timing', {})
        configure = format_duration(timing.get('configure_seconds', 0))
        build = format_duration(timing.get('build_seconds', 0))
        test = format_duration(timing.get('test_seconds', 0))
        total = format_duration(timing.get('total_seconds', 0))
        
        print(f"| {date_str} | {build_type:8} | {ccache:6} | {configure:9} | {build:5} | {test:4} | {total:5} |")
    
    print()

def main():
    parser = argparse.ArgumentParser(description='Analyze build performance benchmark results')
    parser.add_argument('--directory', '-d', default='.', 
                       help='Directory containing benchmark result files (default: current directory)')
    parser.add_argument('--recent', '-r', type=int, default=5,
                       help='Number of recent results to show in trend (default: 5)')
    
    args = parser.parse_args()
    
    results = load_benchmark_results(args.directory)
    
    if not results:
        print("No benchmark results found in the specified directory.")
        print("Run ./tools/benchmark_build.sh to generate benchmark data.")
        return
    
    analyze_performance_trend(results)
    generate_recent_trend(results, args.recent)
    
    # Show recommendations
    print("💡 Recommendations:")
    
    ccache_results = [r for r in results if r.get('use_ccache', False)]
    no_ccache_results = [r for r in results if not r.get('use_ccache', False)]
    
    if ccache_results and no_ccache_results:
        ccache_build_times = [r['timing']['build_seconds'] for r in ccache_results 
                             if 'timing' in r and 'build_seconds' in r['timing']]
        no_ccache_build_times = [r['timing']['build_seconds'] for r in no_ccache_results 
                                if 'timing' in r and 'build_seconds' in r['timing']]
        
        if ccache_build_times and no_ccache_build_times:
            ccache_mean = statistics.mean(ccache_build_times)
            no_ccache_mean = statistics.mean(no_ccache_build_times)
            
            if ccache_mean < no_ccache_mean:
                improvement = ((no_ccache_mean - ccache_mean) / no_ccache_mean) * 100
                print(f"  • Keep using ccache - it's providing {improvement:.1f}% build time improvement")
                print(f"  • Consider increasing ccache size if cache hit rate is low")
            else:
                print(f"  • ccache might not be effective for this project size")
                print(f"  • Consider profiling compilation bottlenecks")
    
    if results:
        latest = results[-1]
        build_time = latest.get('timing', {}).get('build_seconds', 0)
        
        if build_time > 300:  # 5 minutes
            print(f"  • Build time ({format_duration(build_time)}) is quite long")
            print(f"  • Consider enabling unity builds or precompiled headers")
            print(f"  • Review if all external dependencies are necessary")
        elif build_time > 120:  # 2 minutes  
            print(f"  • Build time ({format_duration(build_time)}) could be optimized")
            print(f"  • Monitor ccache hit rates and tune cache size")
    
    print()

if __name__ == '__main__':
    main()
