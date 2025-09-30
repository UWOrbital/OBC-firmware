from fastapi.testclient import TestClient
from gs.backend.main import app


def test_create_user():
    with TestClient(app) as client:
        json_obj = {
            "call_sign": "KEVWAN",
            "email": "kevwan19@gmail.com",
            "first_name": "kevin",
            "last_name": "wan",
            "phone_number": "18008888888",
        }

        res = client.post("/api/v1/aro/user/create_user/", json=json_obj)
        assert res.status_code == 200
        user = res.model_dump().get("user")
        assert user.call_sign == "KEVWAN"
        assert user.email == "kevwan19@gmail.com"
        assert user.first_name == "kevin"
        assert user.last_name == "wan"
        assert user.phone_number == "18008888888"
