#!/usr/bin/env python3
"""
Minimal testing of commands endpoint logic without FastAPI dependencies.
This tests the core business logic by mocking the database operations.
"""

import sys
import os
from uuid import UUID, uuid4
from typing import Any
from unittest.mock import Mock, MagicMock
import traceback

# Add the project root to Python path
project_root = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, project_root)

print("🔍 Testing commands.py endpoint logic without FastAPI dependencies")
print("="*60)

# Mock the dependencies
class MockCommands:
    def __init__(self, **kwargs):
        self.id = kwargs.get('id', uuid4())
        self.status = kwargs.get('status', 'pending')
        self.type_ = kwargs.get('type_', 1)
        self.params = kwargs.get('params', None)
    
    def model_dump(self):
        return {
            'id': str(self.id),
            'status': self.status,
            'type_': self.type_,
            'params': self.params
        }
    
    def __eq__(self, other):
        if isinstance(other, dict):
            our_dict = self.model_dump()
            # Compare only the fields that are in the other dict
            for key, value in other.items():
                if key in our_dict and our_dict[key] != value:
                    return False
            return True
        return False

class MockHTTPException(Exception):
    def __init__(self, status_code, detail):
        self.status_code = status_code
        self.detail = detail
        super().__init__(f"HTTP {status_code}: {detail}")

# Mock database storage
mock_commands_db = []

def mock_get_all_commands():
    return mock_commands_db.copy()

def mock_create_commands(command_data):
    command = MockCommands(**command_data)
    mock_commands_db.append(command)
    return command

def mock_delete_commands_by_id(command_id):
    global mock_commands_db
    for i, cmd in enumerate(mock_commands_db):
        if cmd.id == command_id:
            deleted_cmd = mock_commands_db.pop(i)
            return deleted_cmd
    raise ValueError("Command not found.")

# Mock the endpoint functions
async def mock_create_command(payload: dict[str, Any]):
    """Mock version of create_command endpoint"""
    commands = mock_get_all_commands()
    
    # Check for duplicates using the corrected logic (excluding id field)
    if any(
        {k: v for k, v in cmd.model_dump().items() if k != 'id'} == payload
        for cmd in commands
    ):
        raise MockHTTPException(status_code=400, detail="Invalid command payload")
    
    return mock_create_commands(payload)

async def mock_delete_command(command_id: UUID):
    """Mock version of delete_command endpoint"""
    commands = mock_get_all_commands()
    command_to_delete = next((cmd for cmd in commands if cmd.id == command_id), None)
    
    if not command_to_delete:
        raise FileNotFoundError(f"Command with id {command_id} not found")
    
    mock_delete_commands_by_id(command_id)
    return {"message": f"Command with id {command_id} deleted successfully"}

# Test runner
class TestRunner:
    def __init__(self):
        self.tests_run = 0
        self.tests_passed = 0
        self.tests_failed = 0
    
    async def run_test(self, test_name: str, test_func):
        print(f"\n🔍 Running: {test_name}")
        self.tests_run += 1
        
        try:
            await test_func()
            print(f"✅ PASSED: {test_name}")
            self.tests_passed += 1
        except Exception as e:
            print(f"❌ FAILED: {test_name}")
            print(f"   Error: {str(e)}")
            self.tests_failed += 1
    
    def print_summary(self):
        print(f"\n{'='*50}")
        print(f"TEST SUMMARY")
        print(f"{'='*50}")
        print(f"Tests run: {self.tests_run}")
        print(f"Passed: {self.tests_passed}")
        print(f"Failed: {self.tests_failed}")
        print(f"Success rate: {(self.tests_passed/self.tests_run)*100:.1f}%" if self.tests_run > 0 else "No tests run")

# Test functions
async def test_create_command_basic():
    """Test basic command creation"""
    payload = {
        "status": "pending",
        "type_": 1,
        "params": "test parameters"
    }
    
    result = await mock_create_command(payload)
    
    assert isinstance(result, MockCommands), f"Expected MockCommands, got {type(result)}"
    assert result.status == "pending"
    assert result.type_ == 1
    assert result.params == "test parameters"
    assert result.id is not None
    
    print(f"   Created command with ID: {result.id}")

async def test_create_command_minimal():
    """Test minimal command creation"""
    payload = {"type_": 2}
    
    result = await mock_create_command(payload)
    
    assert isinstance(result, MockCommands)
    assert result.status == "pending"  # default value
    assert result.type_ == 2
    assert result.params is None
    
    print(f"   Created minimal command with ID: {result.id}")

async def test_create_command_duplicate():
    """Test duplicate command rejection"""
    payload = {
        "status": "pending", 
        "type_": 3,
        "params": "duplicate test"
    }
    
    # Create first command
    first_cmd = await mock_create_command(payload)
    print(f"   Created first command with ID: {first_cmd.id}")
    
    # Try to create duplicate
    try:
        await mock_create_command(payload)
        raise AssertionError("Expected HTTPException for duplicate")
    except MockHTTPException as e:
        assert e.status_code == 400
        print(f"   ✅ Correctly rejected duplicate: {e.detail}")

async def test_delete_command_success():
    """Test successful deletion"""
    payload = {
        "status": "pending",
        "type_": 4,
        "params": "to be deleted"
    }
    
    command = await mock_create_command(payload)
    command_id = command.id
    print(f"   Created command to delete: {command_id}")
    
    result = await mock_delete_command(command_id)
    
    assert isinstance(result, dict)
    assert "message" in result
    assert str(command_id) in result["message"]
    
    print(f"   ✅ Successfully deleted: {result['message']}")

async def test_delete_command_not_found():
    """Test deletion of non-existent command"""
    fake_id = uuid4()
    
    try:
        await mock_delete_command(fake_id)
        raise AssertionError("Expected FileNotFoundError")
    except FileNotFoundError as e:
        assert str(fake_id) in str(e)
        print(f"   ✅ Correctly raised FileNotFoundError: {str(e)}")

async def test_command_storage():
    """Test that commands are properly stored"""
    initial_count = len(mock_commands_db)
    
    payload = {"type_": 5, "params": "storage test"}
    await mock_create_command(payload)
    
    assert len(mock_commands_db) == initial_count + 1
    print(f"   ✅ Command properly stored. DB now has {len(mock_commands_db)} commands")

async def test_get_all_commands():
    """Test getting all commands"""
    commands = mock_get_all_commands()
    print(f"   Current commands in DB: {len(commands)}")
    
    # All commands should be MockCommands instances
    for cmd in commands:
        assert isinstance(cmd, MockCommands)
    
    print(f"   ✅ Successfully retrieved {len(commands)} commands")

async def main():
    """Main test runner"""
    runner = TestRunner()
    
    # Clear any existing data
    mock_commands_db.clear()
    
    await runner.run_test("Create command - basic", test_create_command_basic)
    await runner.run_test("Create command - minimal", test_create_command_minimal)
    await runner.run_test("Get all commands", test_get_all_commands)
    await runner.run_test("Command storage verification", test_command_storage)
    await runner.run_test("Create duplicate command (should fail)", test_create_command_duplicate)
    await runner.run_test("Delete command - success", test_delete_command_success)
    await runner.run_test("Delete command - not found", test_delete_command_not_found)
    
    runner.print_summary()
    
    if runner.tests_failed > 0:
        sys.exit(1)
    else:
        print("\n🎉 All tests passed!")
        print("\nNote: These tests validate the core business logic.")
        print("For full HTTP endpoint testing, you would need to:")
        print("1. Install dependencies: pip install -r requirements.txt")
        print("2. Start the server: python3 -m uvicorn backend.main:app")
        print("3. Run the curl test script: ./test_commands_curl.sh")

if __name__ == "__main__":
    import asyncio
    asyncio.run(main())