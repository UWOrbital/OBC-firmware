"""
Cache performance test
"""
import time


def test_cache_performance(client):
    """Test cache performance with multiple fibonacci values"""
    
    test_values = [20, 30, 35]
    
    print("\n" + "="*70)
    print("CACHE PERFORMANCE TEST")
    print("="*70)
    
    for test_value in test_values:
        print(f"\nTesting fibonacci({test_value})...")
        print("-" * 50)
        
        # Time uncached requests
        print("Uncached requests:")
        uncached_times = []
        for i in range(3):
            start = time.time()
            response = client.get(f"/api/cache_testing/slow/{test_value}")
            elapsed = time.time() - start
            uncached_times.append(elapsed)
            print(f"  Request {i+1}: {elapsed:.4f}s")
            assert response.status_code == 200
        
        # Time cached requests
        print("Cached requests:")
        cached_times = []
        for i in range(3):
            start = time.time()
            response = client.get(f"/api/cache_testing/slow-cached/{test_value}")
            elapsed = time.time() - start
            cached_times.append(elapsed)
            print(f"  Request {i+1}: {elapsed:.4f}s")
            assert response.status_code == 200
        
        # Calculate averages
        avg_uncached = sum(uncached_times) / len(uncached_times)
        avg_cached_all = sum(cached_times) / len(cached_times)  # Including first call
        avg_cached_successive = sum(cached_times[1:]) / len(cached_times[1:]) if len(cached_times) > 1 else 0  # Only successive calls
        
        improvement_all = avg_uncached / avg_cached_all if avg_cached_all > 0 else float('inf')
        improvement_successive = avg_uncached / avg_cached_successive if avg_cached_successive > 0 else float('inf')
        
        # Show results for this test value
        print(f"\nResults for fibonacci({test_value}):")
        print(f"  Average uncached: {avg_uncached:.4f}s")
        print(f"  Average cached (all requests): {avg_cached_all:.4f}s") 
        print(f"  Average cached (successive only): {avg_cached_successive:.4f}s")
        print(f"  Speed improvement (all): {improvement_all:.1f}x faster")
        print(f"  Speed improvement (successive): {improvement_successive:.1f}x faster")
        print(f"  Time saved (all): {(avg_uncached - avg_cached_all)*1000:.1f}ms per request")
        print(f"  Time saved (successive): {(avg_uncached - avg_cached_successive)*1000:.1f}ms per request")
        
        # Cache should be faster (at least after first request)
        assert avg_cached_successive < avg_uncached, f"Successive cached calls should be faster for fib({test_value})"
    
    print("\n" + "="*70)
    print("✓ All cache performance tests passed!")
    print("="*70)