from enum import Enum, StrEnum, auto


class CommandStatus(StrEnum):
    """
    Represents the possible states that a command can be in
    """

    PENDING = auto()  # Command was created in the db but not yet sent to the OBC
    SCHEDULED = auto()  # Command is scheduled to be sent to the OBC
    SENT = auto()  # Command was sent to the OBC
    CANCELLED = auto()  # Command was cancelled by MCC or an ARO. This is a final state of a command
    FAILED = auto()  # Command failed to complete. This is a final state of a command
    COMPLETED = (
        auto()
    )  # Command executed successfully. this should be the final state of a command if all was successful


class SessionStatus(StrEnum):
    """
    Represents the possible states that a session can be in
    """

    PENDING = auto()  # Initial state of a session. Optional or can start at SCHEDULED status
    SCHEDULED = auto()  # Session has been scheduled. GS has not received any data yet but the start time is known
    ONGOING = auto()  # Session has been started. GS is receiving data
    COMPLETED = auto()  # Session is complete. GS has received all the data for the session. Final state of session


class MainPacketType(Enum):
    """
    Represents the type of packets that can be transmited/received
    """

    UPLINK = auto()
    DOWNLINK = auto()
