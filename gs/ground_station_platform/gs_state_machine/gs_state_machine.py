from enum import Enum

class GroundStationState(Enum):
  GS_STATE_DISCONNECTED = 0
  GS_STATE_SEND_CONN = 1
  GS_STATE_AWAIT_CONN_ACK = 2
  GS_STATE_UPLINK_COMMANDS = 3
  GS_STATE_DOWNLINK_TELEM = 4
  GS_STATE_SEND_DISC_ACK = 5
  GS_STATE_ERROR_CLEANUP = 6

class GroundStationEvent(Enum):
  GS_EVENT_SENT_CONN = 0
  GS_EVENT_UPINK_BEGIN = 1
  GS_EVENT_UPLINK_FINISHED = 2

  # Disconnect ack events
  GS_EVENT_DISC_ACK_SENT = 3
  GS_EVENT_DISC_RECEIVED = 4

  # Initial connection ack events
  GS_EVENT_ACK_RECIEVED = 5
  GS_EVENT_NO_ACK = 6
  GS_EVENT_ERROR_RESET = 7

currentState : GroundStationState = GroundStationState.GS_STATE_DISCONNECTED

def initGroundStationInterface():
  currentState = GroundStationState.GS_STATE_DISCONNECTED

def setNextState(gsEvent : GroundStationEvent) -> GroundStationState:
  prevState : GroundStationState = currentState

  match currentState:
    case GroundStationState.GS_STATE_DISCONNECTED:
      match gsEvent:
        case GroundStationEvent.GS_EVENT_UPINK_BEGIN:
          currentState = GroundStationState.GS_STATE_SEND_CONN
        case _:
          currentState = GroundStationState.GS_STATE_ERROR_CLEANUP

    case GroundStationState.GS_STATE_SEND_CONN:
      match gsEvent:
        case GroundStationEvent.GS_EVENT_SENT_CONN:
          currentState = GroundStationState.GS_STATE_AWAIT_CONN_ACK
        case _:
          currentState = GroundStationState.GS_STATE_ERROR_CLEANUP

    case GroundStationState.GS_STATE_AWAIT_CONN_ACK:
        match gsEvent:
          case GroundStationEvent.GS_EVENT_ACK_RECIEVED:
            currentState = GroundStationState.GS_STATE_UPLINK_COMMANDS
          case _:  # Including no ack recieved
            currentState = GroundStationState.GS_STATE_ERROR_CLEANUP

    case GroundStationState.GS_STATE_UPLINK_COMMANDS:
        match gsEvent:
          case GroundStationEvent.GS_EVENT_UPLINK_FINISHED:
            currentState = GroundStationState.GS_STATE_DOWNLINK_TELEM
          case _:  # Including no ack recieved
            currentState = GroundStationState.GS_STATE_ERROR_CLEANUP

    case GroundStationState.GS_STATE_DOWNLINK_TELEM:
        match gsEvent:
          case GroundStationEvent.GS_EVENT_DISC_RECEIVED:
            currentState = GroundStationState.GS_STATE_SEND_DISC_ACK
          case _:  # Including no ack recieved
            currentState = GroundStationState.GS_STATE_ERROR_CLEANUP

    case GroundStationState.GS_STATE_SEND_DISC_ACK:
      match gsEvent:
        case GroundStationEvent.GS_EVENT_DISC_ACK_SENT:
          currentState = GroundStationState.GS_STATE_DISCONNECTED
        case _:
          currentState = GroundStationState.GS_STATE_ERROR_CLEANUP

    case GroundStationState.GS_STATE_ERROR_CLEANUP:
      currentState = GroundStationState.GS_STATE_DISCONNECTED

  if (currentState == GroundStationState.GS_STATE_ERROR_CLEANUP):
    pass ## log(prevState, gsEvent and timestamp)
  return currentState
