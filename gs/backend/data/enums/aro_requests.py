from enum import Enum, auto


class AroRequestStatus(Enum):
    """
    The possible states that an ARO request can be
    """

    PENDING = auto()  # note: 'Command was created in the db but not yet sent to the OBC'
    SCHEDULED = auto()  # note: 'Command was sent to OBC'
    ONGOING = auto()  # note: 'Command is executing on OBC'
    CANCELLED = auto()  # note: 'Command was cancelled by MCC. This is a final state of a command'
    FAILED = auto()  # note: 'Command failed to complete. This is a final state of a command'
    COMPLETED = (
        auto()
    )  # note: 'Command executed successfully. this should be the final state of a command if all was successful'
