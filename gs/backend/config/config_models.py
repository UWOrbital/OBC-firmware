from collections.abc import Sequence

from pydantic import BaseModel


class CORSConfig(BaseModel):
    """model for CORS configuration"""

    allow_origins: Sequence[str]
    allow_credentials: bool
    allow_methods: Sequence[str]
    allow_headers: Sequence[str]


class LoggerConfig(BaseModel):
    """model for logger configuration"""

    excluded_endpoints: Sequence[str]
