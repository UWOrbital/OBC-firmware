from fastapi import FastAPI
from fastapi.testclient import TestClient
from gs.backend.exceptions.exception_handlers import setup_exception_handlers
from gs.backend.exceptions.exceptions import (
    BaseOrbitalError,
    DatabaseError,
    InvalidArgumentError,
    InvalidStateError,
    NotFoundError,
    ServiceError,
    SunPositionError,
    UnauthorizedError,
    UnknownError,
)
from starlette.status import (
    HTTP_400_BAD_REQUEST,
    HTTP_401_UNAUTHORIZED,
    HTTP_404_NOT_FOUND,
    HTTP_409_CONFLICT,
    HTTP_422_UNPROCESSABLE_ENTITY,
)


def test_custom_exceptions():
    app = FastAPI()
    setup_exception_handlers(app)

    @app.get("/test_base_orbital_exceptions")
    async def test_custom_exceptions():
        raise BaseOrbitalError("Test BaseOrbitalError")

    @app.get("/test_database_exceptions")
    async def test_database_exceptions():
        raise DatabaseError("Test DatabaseError")

    @app.get("/test_invalid_argument_exceptions")
    async def test_invalid_argument_exceptions():
        raise InvalidArgumentError("Test InvalidArgumentError")

    @app.get("/test_invalid_state_exceptions")
    async def test_invalid_state_exceptions():
        raise InvalidStateError("Test InvalidStateError")

    @app.get("/test_not_found_exceptions")
    async def test_not_found_exceptions():
        raise NotFoundError("Test NotFoundError")

    @app.get("/test_service_exceptions")
    async def test_service_exceptions():
        raise ServiceError("Test ServiceError")

    @app.get("/test_sun_position_exceptions")
    async def test_sun_position_exceptions():
        raise SunPositionError("Test SunPositionError")

    @app.get("/test_unauthorized_exceptions")
    async def test_unauthorized_exceptions():
        raise UnauthorizedError("Test UnauthorizedError")

    @app.get("/test_unknown_exceptions")
    async def test_unknown_exceptions():
        raise UnknownError("Test UnknownError")

    fastapi_test_client = TestClient(app)
    # Test BaseOrbitalError
    response = fastapi_test_client.get("/test_base_orbital_exceptions")
    assert response.status_code == HTTP_400_BAD_REQUEST
    assert response.json().get("message") == "Test BaseOrbitalError"

    # Test DatabaseError
    response = fastapi_test_client.get("/test_database_exceptions")
    assert response.status_code == HTTP_400_BAD_REQUEST
    assert response.json().get("message") == "Test DatabaseError"

    # Test InvalidArgumentError
    response = fastapi_test_client.get("/test_invalid_argument_exceptions")
    assert response.status_code == HTTP_422_UNPROCESSABLE_ENTITY
    assert response.json().get("message") == "Test InvalidArgumentError"

    # Test InvalidStateError
    response = fastapi_test_client.get("/test_invalid_state_exceptions")
    assert response.status_code == HTTP_409_CONFLICT
    assert response.json().get("message") == "Test InvalidStateError"

    # Test NotFoundError
    response = fastapi_test_client.get("/test_not_found_exceptions")
    assert response.status_code == HTTP_404_NOT_FOUND
    assert response.json().get("message") == "Test NotFoundError"

    # Test ServiceError
    response = fastapi_test_client.get("/test_service_exceptions")
    assert response.status_code == HTTP_400_BAD_REQUEST
    assert response.json().get("message") == "Test ServiceError"

    # Test SunPositionError
    response = fastapi_test_client.get("/test_sun_position_exceptions")
    assert response.status_code == HTTP_400_BAD_REQUEST
    assert response.json().get("message") == "Test SunPositionError"

    # Test UnauthorizedError
    response = fastapi_test_client.get("/test_unauthorized_exceptions")
    assert response.status_code == HTTP_401_UNAUTHORIZED
    assert response.json().get("message") == "Test UnauthorizedError"

    # Test UnknownError
    response = fastapi_test_client.get("/test_unknown_exceptions")
    assert response.status_code == HTTP_400_BAD_REQUEST
    assert response.json().get("detail") == "Test UnknownError"
