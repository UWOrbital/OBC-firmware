from state_enums import StateMachineStates
from state_transition_enums import StateTransition


class StateMachine:
    """
    Ground station state machine.
    """

    def __init__(self) -> None:
        """
        Initialize the state machine.

        Sets the initial state to DISCONNECTED and transitional_state to None.
        """
        self.state = StateMachineStates.DISCONNECTED
        self.transitional_state: StateTransition = StateTransition.DEFAULT

    def switch_state(self, transitional_state: StateTransition) -> None:
        """
        :params transitional_state: The transitional state of the state machine, used to update state
        """
        self.transitional_state = transitional_state

        match self.state:
            case StateMachineStates.DISCONNECTED:
                match self.transitional_state:
                    case StateTransition.ENTER_EMERGENCY:
                        self.state = StateMachineStates.ENTERING_EMERGENCY
                    case StateTransition.BEGIN_UPLINK:
                        self.state = StateMachineStates.ATTEMPTING_CONNECTION
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.ATTEMPTING_CONNECTION:
                match self.transitional_state:
                    case StateTransition.CONNECTION_ESTABLISHED:
                        self.state = StateMachineStates.AWAITING_ACK
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.AWAITING_ACK:
                match self.transitional_state:
                    case StateTransition.ACK_RECIEVED:
                        self.state = StateMachineStates.UPLINKING
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.UPLINKING:
                match self.transitional_state:
                    case StateTransition.UPLINK_FINISHED:
                        self.state = StateMachineStates.DOWNLINKING
                    case StateTransition.DISCONNECTING:
                        self.state = StateMachineStates.AWAITING_DISCONNECT
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.AWAITING_DISCONNECT:
                match self.transitional_state:
                    case StateTransition.DISCONNECT_CMD_RECIEVED:
                        self.state = StateMachineStates.SEND_DISCONNECT_ACK
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.SEND_DISCONNECT_ACK:
                match self.transitional_state:
                    case StateTransition.DISCONNECT_COMPLETE:
                        self.state = StateMachineStates.DISCONNECTED
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.DOWNLINKING:
                match self.transitional_state:
                    case StateTransition.DOWNLINKING_FINISHED:
                        self.state = StateMachineStates.UPLINKING
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.ENTERING_EMERGENCY:
                match self.transitional_state:
                    case StateTransition.EMERGENCY_INITIATED:
                        self.state = StateMachineStates.AWAITING_CONNECTION
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.AWAITING_CONNECTION:
                match self.transitional_state:
                    case StateTransition.CONNECTION_RECIEVED:
                        self.state = StateMachineStates.SEND_CONNECTION_ACK
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.SEND_CONNECTION_ACK:
                match self.transitional_state:
                    case StateTransition.CONNECTION_ACK_SENT:
                        self.state = StateMachineStates.EMERGENCY_UPLINK
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR

            case StateMachineStates.EMERGENCY_UPLINK:
                match self.transitional_state:
                    case StateTransition.EMERGENCY_UPLINK_FINISHED:
                        self.state = StateMachineStates.DISCONNECTED
                    case StateTransition.ERROR:
                        self.state = StateMachineStates.DISCONNECTED
                    case _:
                        self.state = StateMachineStates.SERVER_SIDE_ERROR
