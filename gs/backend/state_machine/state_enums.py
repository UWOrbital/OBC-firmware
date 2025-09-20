from enum import StrEnum, auto


class StateMachineStates(StrEnum):
    """
    The states of the ground station state machine
    Defaults to disconnected
    """

    DISCONNECTED = auto()

    ATTEMPTING_CONNECTION = auto()

    UPLINKING = auto()
    DOWNLINKING = auto()

    AWAITING_DISCONNECT = auto()
    AWAITING_ACK = auto()
    AWAITING_CONNECTION = auto()

    SEND_DISCONNECT_ACK = auto()
    SEND_CONNECTION_ACK = auto()

    ENTERING_EMERGENCY = auto()
    EMERGENCY_UPLINK = auto()

    SERVER_SIDE_ERROR = auto()
