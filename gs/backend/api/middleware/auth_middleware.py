from fastapi import Request, Response
from starlette.middleware.base import BaseHTTPMiddleware, RequestResponseEndpoint


class AuthMiddleware(BaseHTTPMiddleware):
    """Middleware that is responsible for authenticating the user"""

    async def dispatch(self, request: Request, call_next: RequestResponseEndpoint) -> Response:
        """Authenticates the user"""
        # TODO: Implement authentication here
        response = await call_next(request)
        return response
