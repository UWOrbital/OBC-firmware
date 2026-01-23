from enum import StrEnum, auto


class AROAuthToken(StrEnum):
    """
    The possible states that an AROAuthToken can be.
    eg. types of authentication types.
    
    GOOGLE_OAUTH: Token issued after Google OAuth authentication
    EMAIL_PASSWORD: Token issued after email/password authentication
    REFRESH: Refresh token for extending sessions (future use)
    """

    # TODO Implmement proper states (in progress)
    GOOGLE_OAUTH = auto()
    EMAIL_PASSWORD = auto()
    REFRESH = auto()
