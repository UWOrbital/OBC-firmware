"""
Cache testing endpoints for performance comparison
"""
import time
from typing import Dict, Any
from fastapi import APIRouter
from fastapi_cache.decorator import cache

cache_test_router = APIRouter(tags=["Cache Testing"])

def slow_computation(n: int) -> Dict[str, Any]:
    """
    Regular fibonacci computation for testing performance without cache
    """
    start_time = time.time()
    
    # Simple fibonacci calculation
    def fib(num: int) -> int:
        if num <= 1:
            return num
        return fib(num - 1) + fib(num - 2)
    
    result = fib(min(n, 35))
    computation_time = time.time() - start_time
    
    return {
        "input": n,
        "result": result,
        "computation_time": round(computation_time, 4)
    }

@cache_test_router.get("/slow/{n}")
async def slow_no_cache(n: int):
    """Slow computation without caching"""
    return slow_computation(n)

@cache_test_router.get("/slow-cached/{n}")
@cache(expire=60)  # Cache for 1 minute
async def slow_with_cache(n: int):
    """Slow computation with caching"""
    return slow_computation(n)