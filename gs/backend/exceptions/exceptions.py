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
    """

    def __init__(self, message: str = "Service exception") -> None:
        super().__init__(message)


class NotFoundError(ServiceError):
    """
    @brief Exception raised when an item is not found
    """

    def __init__(self, message: str = "Item not found") -> None:
        super().__init__(message)


class InvalidArgumentError(BaseOrbitalError):
    """
    @brief Exception raised when an invalid argument is provided
    """

    def __init__(self, message: str = "The argument provided was invalid") -> None:
        super().__init__(message)


class InvalidStateError(BaseOrbitalError):
    """
    @brief Exception raised when an operation is performed in an invalid state
    """

    def __init__(self, message: str = "Invalid state") -> None:
        super().__init__(message)


class DatabaseError(BaseOrbitalError):
    """
    @brief Exception raised for database-related errors
    """

    def __init__(self, message: str = "There was a database error") -> None:
        super().__init__(message)


class UnauthorizedError(BaseOrbitalError):
    """
    @brief Exception raised when a user is not authorized to perform an action
    """

    def __init__(self, message: str = "The current user is not authorized to perform this action") -> None:
        super().__init__(message)


class UnknownError(BaseOrbitalError):
    """
    @brief Exception raised for unexpected errors
    """

    def __init__(self, message: str = "An unexpected error occurred") -> None:
        super().__init__(message)


class SunPositionError(BaseOrbitalError):
    """
    @brief Exception raised when an error occurs in sun position data generation
    """

    def __init__(self, message: str = "There was an error when trying to generate the sun position data") -> None:
        super().__init__(message)
