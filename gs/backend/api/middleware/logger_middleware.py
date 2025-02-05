from fastapi import Request, Response
from starlette.middleware.base import BaseHTTPMiddleware, RequestResponseEndpoint


class LoggerMiddleware(BaseHTTPMiddleware):
    """Middleware that logs the request and response"""

    async def dispatch(self, request: Request, call_next: RequestResponseEndpoint) -> Response:
        """Logs the request and response"""
        # TODO: Add logging here
        response = await call_next(request)
        return response
