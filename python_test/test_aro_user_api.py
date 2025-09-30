import pytest
from fastapi.testclient import TestClient
from gs.backend.main import app


@pytest.fixture
def client():
    return TestClient(app)


# Test data for user 1
@pytest.fixture
def user1_data():
    return {
        "call_sign": "ABCDEF",
        "email": "bob@test.com",
        "first_name": "Bob",
        "last_name": "Smith",
        "phone_number": "123456789",
    }


# Test data for user 2
@pytest.fixture
def user2_data():
    return {
        "call_sign": "KEVWAN",
        "email": "kevian@gmail.com",
        "first_name": "kevin",
        "last_name": "wan",
        "phone_number": "8888888888",
    }


# Test creating user1
@pytest.fixture
def test_user1_creation(client, user1_data):
    response = client.post("/api/v1/aro/user/", json=user1_data, headers={"Content-Type": "application/json"})

    assert response.status_code == 200
    user = response.json()["data"]
    assert user["email"] == user1_data["email"]
    assert user["call_sign"] == user1_data["call_sign"]
    assert user["first_name"] == user1_data["first_name"]
    assert user["last_name"] == user1_data["last_name"]
    assert user["phone_number"] == user1_data["phone_number"]

    return user


# Test creating user2
@pytest.fixture
def test_user2_creation(client, user2_data):
    response = client.post("/api/v1/aro/user/", json=user2_data, headers={"Content-Type": "application/json"})

    assert response.status_code == 200
    user = response.json()["data"]
    assert user["email"] == user2_data["email"]
    assert user["call_sign"] == user2_data["call_sign"]
    assert user["first_name"] == user2_data["first_name"]
    assert user["last_name"] == user2_data["last_name"]
    assert user["phone_number"] == user2_data["phone_number"]

    return user


# Test updating user1 (depends on test_user1_creation)
@pytest.fixture
def test_user1_update(client, test_user1_creation):
    user_id = test_user1_creation["id"]
    update_data = {
        "call_sign": "123456",
        "email": "bob2@test.com",
        "first_name": "Rob",
        "last_name": "Smith",
        "phone_number": "234567890",
    }
    res = client.put(f"/api/v1/aro/user/{user_id}", json=update_data, headers={"Content-Type": "application/json"})

    assert res.status_code == 200
    updated_user = res.json()["data"]
    assert updated_user["email"] == update_data["email"]
    assert updated_user["call_sign"] == update_data["call_sign"]
    assert updated_user["first_name"] == update_data["first_name"]
    assert updated_user["last_name"] == update_data["last_name"]
    assert updated_user["phone_number"] == update_data["phone_number"]

    return updated_user


# Test getting all users (after creating user1 and user2, and updating user1 to ensure creation and update work oncorrect user objects)
@pytest.fixture
def test_get_users(client, test_user1_update, test_user2_creation):
    res = client.get("/api/v1/aro/user/")

    assert res.status_code == 200
    all_users = res.json()["data"]
    assert len(all_users) == 2

    # Check user1
    user1_id = test_user1_update["id"]
    user1_from_response = next(user for user in all_users if user["id"] == user1_id)
    assert user1_from_response["call_sign"] == test_user1_update["call_sign"]
    assert user1_from_response["email"] == test_user1_update["email"]
    assert user1_from_response["first_name"] == test_user1_update["first_name"]
    assert user1_from_response["last_name"] == test_user1_update["last_name"]
    assert user1_from_response["phone_number"] == test_user1_update["phone_number"]

    # Check user2
    user2_id = test_user2_creation["id"]
    user2_from_response = next(user for user in all_users if user["id"] == user2_id)
    assert user2_from_response["email"] == test_user2_creation["email"]
    assert user2_from_response["call_sign"] == test_user2_creation["call_sign"]
    assert user2_from_response["first_name"] == test_user2_creation["first_name"]
    assert user2_from_response["last_name"] == test_user2_creation["last_name"]
    assert user2_from_response["phone_number"] == test_user2_creation["phone_number"]


# Test deleting user1 (after test_get_users to ensure both users exist)
def test_user1_deletion(client, test_user1_update, test_user2_creation, test_get_users):
    user_id = test_user1_update["id"]
    res = client.delete(f"/api/v1/aro/user/{user_id}", headers={"Content-Type": "application/json"})

    assert res.status_code == 200
    all_users = res.json()["data"]
    assert len(all_users) == 1
    assert all_users[0]["id"] != user_id  # Ensure user1 is deleted
    assert all_users[0]["id"] == test_user2_creation["id"]
    assert all_users[0]["email"] == test_user2_creation["email"]
    assert all_users[0]["call_sign"] == test_user2_creation["call_sign"]
    assert all_users[0]["first_name"] == test_user2_creation["first_name"]
    assert all_users[0]["last_name"] == test_user2_creation["last_name"]
    assert all_users[0]["phone_number"] == test_user2_creation["phone_number"]

    return all_users
