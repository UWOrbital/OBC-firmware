from fastapi import Request, Response
from starlette.middleware.base import BaseHTTPMiddleware, RequestResponseEndpoint
from loguru import logger
from time import time
from sys import getsizeof

#add any endpoints we don't want logged here, e.g "/example/endpoint"
excluded_endpoints = []
class LoggerMiddleware(BaseHTTPMiddleware):
    """Middleware that logs the request and response"""

    async def dispatch(self, request: Request, call_next: RequestResponseEndpoint) -> Response:
        start_time = time()
        response = await call_next(request)
        process_time = time() - start_time
        
        if request.url.path in excluded_endpoints:
            return response
        
        request_body = await request.body()
        request_size = getsizeof(request_body)
        # TODO: update this based on userID header name
        request_userID = request.headers.get("userID", "Anonymous")
        request_params = dict(request.query_params)

        logger.info(f"REQUEST | Method: {request.method} | URL: {request.url.path} | User id: {request_userID} | Params: {request_params} | Size: {request_size} bytes.")
        
        if response.status_code >= 500:
            logger_severity = logger.critical
        elif response.status_code >= 400:
            logger_severity = logger.error
        else:
            logger_severity = logger.info
        
        response_body = b''.join([chunk async for chunk in response.body_iterator])
        response_size = getsizeof(response_body)

        logger_severity(f"RESPONSE | Status: {response.status_code} | Response: {response_body.decode(errors="ignore")} | Size: {response_size} bytes | Time Elasped: {process_time:.3f}.")
        
        return Response(
            content=response_body, 
            status_code=response.status_code, 
            headers=dict(response.headers), 
            media_type=response.media_type
        )
