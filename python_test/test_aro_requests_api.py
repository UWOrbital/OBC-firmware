from datetime import datetime
from decimal import Decimal
from uuid import UUID

import pytest
from fastapi.testclient import TestClient
from gs.backend.data.data_wrappers.aro_wrapper.aro_request_wrapper import add_request
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper import add_user
from gs.backend.data.enums.aro_requests import ARORequestStatus
from gs.backend.main import app


@pytest.fixture
def client():
    return TestClient(app)


# Create user for request 1
@pytest.fixture
def test_user1():
    user = add_user(
        call_sign="TEST01",
        email="test1@example.com",
        f_name="Test",
        l_name="User1",
        phone_number="1234567890",
    )
    return user


# Create user for request 2
@pytest.fixture
def test_user2():
    user = add_user(
        call_sign="TEST02",
        email="test2@example.com",
        f_name="Test",
        l_name="User2",
        phone_number="0987654321",
    )
    return user


# Test data for request 1
@pytest.fixture
def request1_data(test_user1):
    dt_str = "2021-01-01T00:00:00Z"
    dt = datetime.fromisoformat(dt_str.replace("Z", "+00:00"))
    return {
        "aro_id": test_user1.id,
        "long": Decimal("123.456"),
        "lat": Decimal("49.282"),
        "created_on": dt,
        "request_sent_obc": dt,
        "taken_date": dt,
        "transmission": dt,
        "status": ARORequestStatus.PENDING,
    }


# Test data for request 2
@pytest.fixture
def request2_data(test_user2):
    dt_str = "2021-01-01T00:00:00Z"
    dt = datetime.fromisoformat(dt_str.replace("Z", "+00:00"))
    return {
        "aro_id": test_user2.id,
        "long": Decimal("123.456"),
        "lat": Decimal("49.282"),
        "created_on": dt,
        "request_sent_obc": dt,
        "taken_date": dt,
        "transmission": dt,
        "status": ARORequestStatus.COMPLETED,
    }


# Add request1 to database
@pytest.fixture
def test_request1_creation(client, request1_data):
    request = add_request(
        aro_id=request1_data["aro_id"],
        long=request1_data["long"],
        lat=request1_data["lat"],
        created_on=request1_data["created_on"],
        request_sent_obc=request1_data["request_sent_obc"],
        taken_date=request1_data["taken_date"],
        transmission=request1_data["transmission"],
        status=request1_data["status"],
    )
    return request


# Add request2 to database
@pytest.fixture
def test_request2_creation(client, request2_data):
    request = add_request(
        aro_id=request2_data["aro_id"],
        long=request2_data["long"],
        lat=request2_data["lat"],
        created_on=request2_data["created_on"],
        request_sent_obc=request2_data["request_sent_obc"],
        taken_date=request2_data["taken_date"],
        transmission=request2_data["transmission"],
        status=request2_data["status"],
    )
    return request


def test_get_all_requests(client, request1_data, request2_data, test_request1_creation, test_request2_creation):
    response = client.get(
        "/api/v1/mcc/requests?count=2&offset=0&filters=pending&filters=completed",
        headers={"Content-Type": "application/json"},
    )
    assert response.status_code == 200
    requests = response.json()["data"]
    assert len(requests) == 2

    request1_response = requests[0]
    assert request1_response["aro_id"] == request1_data["aro_id"]
    assert request1_response["long"] == request1_data["long"]
    assert request1_response["lat"] == request1_data["lat"]
    assert request1_response["created_on"] == request1_data["created_on"]
    assert request1_response["request_sent_obc"] == request1_data["request_sent_obc"]
    assert request1_response["taken_date"] == request1_data["taken_date"]
    assert request1_response["transmission"] == request1_data["transmission"]
    assert request1_response["status"] == request1_data["status"].value

    request2_response = requests[1]
    assert request2_response["aro_id"] == request2_data["aro_id"]
    assert request2_response["long"] == request2_data["long"]
    assert request2_response["lat"] == request2_data["lat"]
    assert request2_response["created_on"] == request2_data["created_on"].isoformat()
    assert request2_response["request_sent_obc"] == request2_data["request_sent_obc"].isoformat()
    assert request2_response["taken_date"] == request2_data["taken_date"].isoformat()
    assert request2_response["transmission"] == request2_data["transmission"].isoformat()
    assert request2_response["status"] == request2_data["status"].value
