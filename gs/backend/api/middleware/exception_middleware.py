from fastapi import Request, Response
from starlette.middleware.base import BaseHTTPMiddleware, RequestResponseEndpoint


class ExceptionMiddleware(BaseHTTPMiddleware):
    """Catches exceptions and returns the appropriate response/error code"""

    async def dispatch(self, request: Request, call_next: RequestResponseEndpoint) -> Response:
        """Catches exceptions and returns the appropriate response/error code"""
        # TODO: Implement exception handling here
        response = await call_next(request)
        return response
