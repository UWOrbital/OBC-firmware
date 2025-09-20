from enum import StrEnum, auto


class StateTransition(StrEnum):
    """
    The transition states of the ground station state machine
    """

    ERROR = auto()
    ACK_RECIEVED = auto()

    BEGIN_UPLINK = auto()
    UPLINK_FINISHED = auto()
    DOWNLINKING_FINISHED = auto()

    DISCONNECTING = auto()
    DISCONNECT_CMD_RECIEVED = auto()
    DISCONNECT_COMPLETE = auto()

    EMERGENCY_INITIATED = auto()
    EMERGENCY_UPLINK_FINISHED = auto()
    ENTER_EMERGENCY = auto()

    CONNECTION_RECIEVED = auto()
    CONNECTION_ACK_SENT = auto()
    CONNECTION_ESTABLISHED = auto()

    DEFAULT = auto()  # NOT USED IN STATE MACHINE
