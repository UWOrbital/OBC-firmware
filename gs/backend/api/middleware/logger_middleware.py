from collections.abc import Awaitable, Callable, Sequence
from datetime import datetime
from sys import getsizeof
from time import time

from fastapi import FastAPI, Request, Response, status
from loguru import logger
from starlette.middleware.base import BaseHTTPMiddleware


class LoggerMiddleware(BaseHTTPMiddleware):
    """Middleware that logs the request and response"""

    def __init__(self, app: FastAPI, excluded_endpoints: Sequence[str] = ()) -> None:
        super().__init__(app)
        self.excluded_endpoints = excluded_endpoints

    async def dispatch(self, request: Request, call_next: Callable[[Request], Awaitable[Response]]) -> Response:
        """Logs the request and response"""
        request_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        start_time = time()
        response = await call_next(request)
        process_time = time() - start_time

        if request.url.path in self.excluded_endpoints:
            return response

        request_body = await request.body()
        request_size = getsizeof(request_body)
        # TODO: update this based on userID header name
        request_user_id = request.headers.get("user_id", "Anonymous")
        request_params = dict(request.query_params)

        logger.info(
            " | ".join(
                [
                    f"REQUEST | Method: {request.method}",
                    f"URL: {request.url.path}",
                    f"User id: {request_user_id}",
                    f"Params: {request_params}",
                    f"Time: {request_time}",
                    f"Size: {request_size} bytes.",
                ]
            )
        )

        if response.status_code >= status.HTTP_500_INTERNAL_SERVER_ERROR:
            logger_severity = logger.critical
        elif response.status_code >= status.HTTP_400_BAD_REQUEST:
            logger_severity = logger.error
        else:
            logger_severity = logger.info

        response_body = b"".join([chunk async for chunk in response.body_iterator])
        response_size = getsizeof(response_body)

        logger_severity(
            " | ".join(
                [
                    f"RESPONSE | Status: {response.status_code}",
                    f"Response: {response_body.decode(errors='ignore')}",
                    f"Size: {response_size} bytes",
                    f"Time Elasped: {process_time:.3f} seconds.",
                ]
            )
        )

        return Response(
            content=response_body,
            status_code=response.status_code,
            headers=dict(response.headers),
            media_type=response.media_type,
        )
