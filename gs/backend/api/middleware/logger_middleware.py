from collections.abc import Awaitable, Callable, Sequence
from datetime import datetime
from sys import getsizeof
from time import time

from fastapi import FastAPI, Request, Response
from loguru import logger
from starlette.middleware.base import BaseHTTPMiddleware
from starlette.status import HTTP_400_BAD_REQUEST, HTTP_500_INTERNAL_SERVER_ERROR


class LoggerMiddleware(BaseHTTPMiddleware):
    """
    @breif Middleware that logs the request and response
    @attribute excluded_endpoints (Sequence[str]) - endpoints that won't be logged
    """

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

        if response.status_code >= HTTP_500_INTERNAL_SERVER_ERROR:
            logger_severity = logger.critical
        elif response.status_code >= HTTP_400_BAD_REQUEST:
            logger_severity = logger.error
        else:
            logger_severity = logger.info

        has_body_iterator = hasattr(response, "body_iterator")

        if has_body_iterator:
            response_body_bytes = b"".join([chunk async for chunk in response.body_iterator])  # type: ignore[attr-defined]
            response_size = str(getsizeof(response_body_bytes))
            response_body = response_body_bytes.decode(errors="ignore")
        else:
            response_body = "Error logging response body"
            response_size = "Error logging response size"

        logger_severity(
            " | ".join(
                [
                    f"RESPONSE | Status: {response.status_code}",
                    f"Response: {response_body}",
                    f"Size (bytes): {response_size}",
                    f"Time Elasped: {process_time:.3f} seconds.",
                ]
            )
        )

        if has_body_iterator:
            return Response(
                content=response_body,
                status_code=response.status_code,
                headers=dict(response.headers),
                media_type=response.media_type,
            )
        else:
            return response
