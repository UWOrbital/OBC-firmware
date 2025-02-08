from enum import StrEnum, auto


class ARORequestStatus(StrEnum):
    """
    The possible states that an ARO request can be
    """

    PENDING = auto()  # Command was created in the db but not yet sent to the OBC
    SCHEDULED = auto()  # Command was sent to OBC
    ONGOING = auto()  # Command is executing on OBC
    CANCELLED = auto()  # Command was cancelled by MCC. This is a final state of a command
    FAILED = auto()  # Command failed to complete. This is a final state of a command
    COMPLETED = (
        auto()
    )  # Command executed successfully. this should be the final state of a command if all was successful
