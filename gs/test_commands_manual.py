#!/usr/bin/env python3
"""
Manual testing script for commands.py endpoints.
This script tests the endpoint functions directly without requiring pytest or a running server.
"""

import sys
import os
from uuid import UUID, uuid4
from typing import Any
import traceback

# Add the project root to Python path
project_root = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, project_root)

# Try to import required modules
try:
    from fastapi import HTTPException
    from gs.backend.api.v1.mcc.endpoints.commands import create_command, delete_command
    from gs.backend.data.tables.transactional_tables import Commands
    from gs.backend.data.enums.transactional import CommandStatus
    print("✅ Successfully imported all required modules")
except ImportError as e:
    print(f"❌ Import error: {e}")
    print("This test requires the project dependencies to be installed.")
    print("Consider installing requirements: pip install -r requirements.txt")
    sys.exit(1)


class TestRunner:
    """Simple test runner for manual testing"""
    
    def __init__(self):
        self.tests_run = 0
        self.tests_passed = 0
        self.tests_failed = 0
    
    async def run_test(self, test_name: str, test_func):
        """Run a single test function"""
        print(f"\n🔍 Running: {test_name}")
        self.tests_run += 1
        
        try:
            await test_func()
            print(f"✅ PASSED: {test_name}")
            self.tests_passed += 1
        except Exception as e:
            print(f"❌ FAILED: {test_name}")
            print(f"   Error: {str(e)}")
            print(f"   Traceback: {traceback.format_exc()}")
            self.tests_failed += 1
    
    def print_summary(self):
        """Print test summary"""
        print(f"\n{'='*50}")
        print(f"TEST SUMMARY")
        print(f"{'='*50}")
        print(f"Tests run: {self.tests_run}")
        print(f"Passed: {self.tests_passed}")
        print(f"Failed: {self.tests_failed}")
        print(f"Success rate: {(self.tests_passed/self.tests_run)*100:.1f}%" if self.tests_run > 0 else "No tests run")


async def test_create_command_basic():
    """Test basic command creation"""
    # Create a valid command payload
    payload = {
        "status": CommandStatus.PENDING,
        "type_": 1,  # Assuming this is a valid command type ID
        "params": "test parameters"
    }
    
    result = await create_command(payload)
    
    # Verify the result
    assert isinstance(result, Commands), f"Expected Commands object, got {type(result)}"
    assert result.status == CommandStatus.PENDING, f"Expected status PENDING, got {result.status}"
    assert result.type_ == 1, f"Expected type_ 1, got {result.type_}"
    assert result.params == "test parameters", f"Expected 'test parameters', got {result.params}"
    assert result.id is not None, "Expected command ID to be generated"
    
    print(f"   Created command with ID: {result.id}")
    return result


async def test_create_command_minimal():
    """Test command creation with minimal payload"""
    # Create a minimal command payload (only required fields)
    payload = {
        "type_": 2  # Assuming this is a valid command type ID
    }
    
    result = await create_command(payload)
    
    # Verify the result
    assert isinstance(result, Commands), f"Expected Commands object, got {type(result)}"
    assert result.status == CommandStatus.PENDING, f"Expected default status PENDING, got {result.status}"
    assert result.type_ == 2, f"Expected type_ 2, got {result.type_}"
    assert result.params is None, f"Expected params to be None, got {result.params}"
    assert result.id is not None, "Expected command ID to be generated"
    
    print(f"   Created minimal command with ID: {result.id}")
    return result


async def test_create_command_duplicate():
    """Test creating a duplicate command (should fail)"""
    # Create the first command
    payload = {
        "status": CommandStatus.PENDING,
        "type_": 3,
        "params": "duplicate test"
    }
    
    first_command = await create_command(payload)
    print(f"   Created first command with ID: {first_command.id}")
    
    # Try to create the same command again (should fail)
    try:
        await create_command(payload)
        raise AssertionError("Expected HTTPException for duplicate command")
    except HTTPException as e:
        assert e.status_code == 400, f"Expected status code 400, got {e.status_code}"
        assert "Invalid command payload" in str(e.detail), f"Expected 'Invalid command payload' in error, got {e.detail}"
        print(f"   ✅ Correctly rejected duplicate command with error: {e.detail}")


async def test_delete_command_success():
    """Test successful command deletion"""
    # First create a command to delete
    payload = {
        "status": CommandStatus.PENDING,
        "type_": 4,
        "params": "to be deleted"
    }
    
    command = await create_command(payload)
    command_id = command.id
    print(f"   Created command to delete with ID: {command_id}")
    
    # Now delete it
    result = await delete_command(command_id)
    
    # Verify the result
    assert isinstance(result, dict), f"Expected dict result, got {type(result)}"
    assert "message" in result, "Expected 'message' key in result"
    assert str(command_id) in result["message"], f"Expected command ID {command_id} in message"
    
    print(f"   ✅ Successfully deleted command: {result['message']}")


async def test_delete_command_not_found():
    """Test deleting a non-existent command"""
    # Generate a random UUID that doesn't exist
    non_existent_id = uuid4()
    
    try:
        await delete_command(non_existent_id)
        raise AssertionError("Expected FileNotFoundError for non-existent command")
    except FileNotFoundError as e:
        assert str(non_existent_id) in str(e), f"Expected UUID {non_existent_id} in error message"
        print(f"   ✅ Correctly raised FileNotFoundError: {str(e)}")


async def test_delete_command_invalid_uuid():
    """Test deleting with an invalid UUID format"""
    try:
        # This should fail at the FastAPI level for UUID validation
        # But since we're testing directly, we need to pass a valid UUID object
        # Let's test with a valid UUID that doesn't exist
        invalid_id = UUID('00000000-0000-0000-0000-000000000000')
        await delete_command(invalid_id)
        raise AssertionError("Expected FileNotFoundError for invalid UUID")
    except FileNotFoundError as e:
        print(f"   ✅ Correctly handled invalid UUID: {str(e)}")


async def main():
    """Main test function"""
    print("🚀 Starting manual tests for commands.py endpoints")
    print("="*50)
    
    runner = TestRunner()
    
    # Run all tests
    await runner.run_test("Create command - basic", test_create_command_basic)
    await runner.run_test("Create command - minimal", test_create_command_minimal)
    await runner.run_test("Create command - duplicate (should fail)", test_create_command_duplicate)
    await runner.run_test("Delete command - success", test_delete_command_success)
    await runner.run_test("Delete command - not found", test_delete_command_not_found)
    await runner.run_test("Delete command - invalid UUID", test_delete_command_invalid_uuid)
    
    # Print summary
    runner.print_summary()
    
    if runner.tests_failed > 0:
        sys.exit(1)
    else:
        print("\n🎉 All tests passed!")


if __name__ == "__main__":
    import asyncio
    asyncio.run(main())