class BaseOrbitalError(Exception):
    """
    @brief Base class for all orbital-related exceptions
    @attribute message (str) - Custom error message providing additional context
    """

    def __init__(self, message: str) -> None:
        super().__init__(message)
        self.message = message


class ServiceError(BaseOrbitalError):
    """
    @brief Exception related to general service errors
    @attribute message (str) - Service exception message
    """

    def __init__(self, message: str = "Service exception") -> None:
        super().__init__(message)


class NotFoundError(ServiceError):
    """
    @brief Exception raised when an item is not found
    @attribute message (str) - Item not found message
    """

    def __init__(self, message: str = "Item not found") -> None:
        super().__init__(message)


class InvalidArgumentError(BaseOrbitalError):
    """
    @brief Exception raised when an invalid argument is provided
    @attribute message (str) - Invalid argument message
    """

    def __init__(self, message: str = "The argument provided was invalid") -> None:
        super().__init__(message)


class InvalidStateError(BaseOrbitalError):
    """
    @brief Exception raised when an operation is performed in an invalid state
    @attribute message (str) - Invalid state message
    """

    def __init__(self, message: str = "Invalid state") -> None:
        super().__init__(message)


class DatabaseError(BaseOrbitalError):
    """
    @brief Exception raised for database-related errors
    @attribute message (str) - Database error message
    """

    def __init__(self, message: str = "There was a database error") -> None:
        super().__init__(message)


class UnauthorizedError(BaseOrbitalError):
    """
    @brief Exception raised when a user is not authorized to perform an action
    @attribute message (str) - Unauthorized error message
    """

    def __init__(self, message: str = "The current user is not authorized to perform this action") -> None:
        super().__init__(message)


class UnknownError(BaseOrbitalError):
    """
    @brief Exception raised for unexpected errors
    @attribute message (str) - Unknown error message
    """

    def __init__(self, message: str = "An unexpected error occurred") -> None:
        super().__init__(message)


class SunPositionError(BaseOrbitalError):
    """
    @brief Exception raised when an error occurs in sun position data generation
    @attribute message (str) - Sun position error message
    """

    def __init__(self, message: str = "There was an error when trying to generate the sun position data") -> None:
        super().__init__(message)
