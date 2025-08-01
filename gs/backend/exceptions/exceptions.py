class BaseOrbitalError(Exception):
    """
    Base class for all orbital-related exceptions

    :param message: Custom error message providing additional context
    :type message: str
    """

    def __init__(self, message: str) -> None:
        super().__init__(message)
        self.message = message


class ServiceError(BaseOrbitalError):
    """
    Exception related to general service errors

    :param message: Service exception message
    :type message: str
    """

    def __init__(self, message: str = "Service exception") -> None:
        super().__init__(message)


class NotFoundError(ServiceError):
    """
    Exception raised when an item is not found

    :param message: Item not found message
    :type message: str
    """

    def __init__(self, message: str = "Item not found") -> None:
        super().__init__(message)


class InvalidArgumentError(BaseOrbitalError):
    """
    Exception raised when an invalid argument is provided

    :param message: Invalid argument message
    :type message: str
    """

    def __init__(self, message: str = "The argument provided was invalid") -> None:
        super().__init__(message)


class InvalidStateError(BaseOrbitalError):
    """
    Exception raised when an operation is performed in an invalid state

    :param message: Invalid state message
    :type message: str
    """

    def __init__(self, message: str = "Invalid state") -> None:
        super().__init__(message)


class DatabaseError(BaseOrbitalError):
    """
    Exception raised for database-related errors

    :param message: Database error message
    :type message: str
    """

    def __init__(self, message: str = "There was a database error") -> None:
        super().__init__(message)


class UnauthorizedError(BaseOrbitalError):
    """
    Exception raised when a user is not authorized to perform an action

    :param message: Unauthorized error message
    :type message: str
    """

    def __init__(self, message: str = "The current user is not authorized to perform this action") -> None:
        super().__init__(message)


class UnknownError(BaseOrbitalError):
    """
    Exception raised for unexpected errors

    :param message: Unknown error message
    :type message: str
    """

    def __init__(self, message: str = "An unexpected error occurred") -> None:
        super().__init__(message)


class SunPositionError(BaseOrbitalError):
    """
    Exception raised when an error occurs in sun position data generation

    :param message: Sun position error message
    :type message: str
    """

    def __init__(self, message: str = "There was an error when trying to generate the sun position data") -> None:
        super().__init__(message)
