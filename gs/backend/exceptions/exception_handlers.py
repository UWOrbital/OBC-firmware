from fastapi import FastAPI, HTTPException, Request, status
from fastapi.responses import JSONResponse

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


def setup_exception_handlers(app: FastAPI) -> None:
    """
    @brief Add exception handlers to the FastAPI app

    @attribute app (FastAPI) - The FastAPI app to add the exception handlers to
    """

    @app.exception_handler(BaseOrbitalError)
    async def base_orbital_exception_handler(request: Request, exc: BaseOrbitalError) -> JSONResponse:
        """
        @brief handle all BaseOrbitalError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (BaseOrbitalError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"message": exc.message},
        )

    @app.exception_handler(ServiceError)
    async def service_exception_handler(request: Request, exc: ServiceError) -> JSONResponse:
        """
        @brief handle all ServiceError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (ServiceError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"message": exc.message},
        )

    @app.exception_handler(NotFoundError)
    async def not_found_exception_handler(request: Request, exc: NotFoundError) -> JSONResponse:
        """
        @brief handle all NotFoundError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (NotFoundError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_404_NOT_FOUND,
            content={"message": exc.message},
        )

    @app.exception_handler(InvalidArgumentError)
    async def invalid_argument_exception_handler(request: Request, exc: InvalidArgumentError) -> JSONResponse:
        """
        @brief handle all InvalidArgumentError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (InvalidArgumentError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            content={"message": exc.message},
        )

    @app.exception_handler(InvalidStateError)
    async def invalid_state_exception_handler(request: Request, exc: InvalidStateError) -> JSONResponse:
        """
        @brief handle all InvalidStateError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (InvalidStateError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT,
            content={"message": exc.message},
        )

    @app.exception_handler(DatabaseError)
    async def data_base_exception_handler(request: Request, exc: DatabaseError) -> JSONResponse:
        """
        @brief handle all DatabaseError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (DatabaseError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"message": exc.message},
        )

    @app.exception_handler(UnauthorizedError)
    async def unauthorized_exception_handler(request: Request, exc: UnauthorizedError) -> JSONResponse:
        """
        @brief handle all UnauthorizedError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (UnauthorizedError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED,
            content={"message": exc.message},
        )

    @app.exception_handler(UnknownError)
    async def unknown_exception_handler(request: Request, exc: UnknownError) -> HTTPException:
        """
        @brief handle all UnknownError exceptions with HTTPException

        @attribute request (Request) - The request that caused the exception
        @attribute exc (UnknownError) - The exception that was raised
        """
        return HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=exc.message,
        )

    @app.exception_handler(SunPositionError)
    async def sun_position_exception_handler(request: Request, exc: SunPositionError) -> JSONResponse:
        """
        @brief handle all SunPositionError exceptions

        @attribute request (Request) - The request that caused the exception
        @attribute exc (SunPositionError) - The exception that was raised
        """
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"message": exc.message},
        )
