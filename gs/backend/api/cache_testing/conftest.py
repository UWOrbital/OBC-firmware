"""
Conftest for cache testing - to test caching without needing database
"""
import pytest
from fastapi import FastAPI
from fastapi.testclient import TestClient
from fastapi_cache import FastAPICache
from fastapi_cache.backends.inmemory import InMemoryBackend
from gs.backend.api.cache_testing.cache_endpoints import cache_test_router


@pytest.fixture
def client():
    """
    Creates a minimal test client for FastAPI app with only cache endpoints
    Does not require database setup
    """
    # Create minimal app with just cache endpoints
    test_app = FastAPI()
    test_app.include_router(cache_test_router, prefix="/api/cache_testing")
    
    # Initialize cache for testing
    FastAPICache.init(InMemoryBackend())
    
    with TestClient(test_app) as test_client:
        yield test_client