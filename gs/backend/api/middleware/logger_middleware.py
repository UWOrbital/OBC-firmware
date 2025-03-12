from collections.abc import Sequence
from datetime import datetime
from sys import getsizeof
from time import time
from typing import Final

from fastapi import FastAPI, Request, Response
from loguru import logger
from starlette.middleware.base import BaseHTTPMiddleware, RequestResponseEndpoint


class LoggerMiddleware(BaseHTTPMiddleware):
    """Middleware that logs the request and response"""

    def __init__(self, app: FastAPI, excluded_endpoints: Sequence[str] = ()) -> None:
        super().__init__(app)
        self.excluded_endpoints = excluded_endpoints

    def add_excluded_endpoints(self, excluded_endpoints: Sequence[str]) -> None:
        """Adds an endpoint to the non-logging list"""
        self.excluded_endpoints.extend(excluded_endpoints)

    async def dispatch(self, request: Request, call_next: RequestResponseEndpoint) -> Response:
        """Logs the request and response"""
        request_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
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

        http_status_code_error_server: Final[int] = 500
        http_status_code_error: Final[int] = 400

        if response.status_code >= http_status_code_error_server:
            logger_severity = logger.critical
        elif response.status_code >= http_status_code_error:
            logger_severity = logger.error
        else:
            logger_severity = logger.info

        is_a_streaming_response = hasattr(response, "body_iterator")

        if is_a_streaming_response:
            response_body = b"".join([chunk async for chunk in response.body_iterator])
        else:
            response_body = await response.body()

        response_size = getsizeof(response_body)

        logger_severity(
            " | ".join(
                [
                    f"RESPONSE | Status: {response.status_code}",
                    f"Response: {response_body.decode(errors='ignore')}",
                    f"Size: {response_size} bytes",
                    f"Time Elasped: {process_time:.3f}.",
                ]
            )
        )

        return Response(
            content=response_body,
            status_code=response.status_code,
            headers=dict(response.headers),
            media_type=response.media_type,
        )
