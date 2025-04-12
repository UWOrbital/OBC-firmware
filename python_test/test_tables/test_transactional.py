from datetime import datetime
from decimal import Decimal
from uuid import uuid4

from gs.backend.data.enums.aro_requests import ARORequestStatus
from gs.backend.data.enums.transactional import CommandStatus, MainPacketType, SessionStatus
from gs.backend.data.tables.aro_user import AROUsers
from gs.backend.data.tables.main import MainCommand, MainTelemetry
from gs.backend.data.tables.transactional import ARORequest, Commands, CommsSession, Packet, Telemetry
from sqlmodel import Session, select

from python_test.conftest import default_comms_session, default_start_time


def test_commands_basic(db_session: Session):
    # Setup the MainCommand table
    main_command1 = MainCommand(id=1, name="Test 1", data_size=1, total_size=2, format="int 7 bytes", params="time")
    db_session.add(main_command1)
    db_session.commit()

    # Make sure that the main command was successfully inserted into the db
    main_commands_query = select(MainCommand)
    main_commands_items = db_session.exec(main_commands_query).all()
    assert len(main_commands_items) == 1

    # Test the commands table
    id = uuid4()
    command1 = Commands(id=id, type_=main_command1.id, params="1234567")
    db_session.add(command1)
    db_session.commit()

    commands_query = select(Commands)
    commands_items = db_session.exec(commands_query).all()

    assert len(commands_items) == 1
    returned_item1 = commands_items[0]
    assert returned_item1.id == id
    assert returned_item1.type_ == 1
    assert returned_item1.params == "1234567"
    assert returned_item1.status == CommandStatus.PENDING

    # Start the delete portion of the test
    db_session.delete(main_command1)
    main_commands_items = db_session.exec(main_commands_query).all()
    assert len(main_commands_items) == 0

    # Make sure it cascades
    commands_items = db_session.exec(commands_query).all()
    assert len(commands_items) == 0


def test_telemetry_basic(db_session: Session):
    # Setup the MainTelemetry table
    main_telemetry = MainTelemetry(id=1, name="Test 1", data_size=1, total_size=2, format="int 7 bytes")
    db_session.add(main_telemetry)
    db_session.commit()

    # Make sure that the main telemetry was successfully inserted into the db
    main_telemetry_query = select(MainTelemetry)
    main_telemetry_items = db_session.exec(main_telemetry_query).all()
    assert len(main_telemetry_items) == 1

    # Test the telemetry table
    id = uuid4()
    telemetry1 = Telemetry(id=id, type_=main_telemetry.id)
    db_session.add(telemetry1)
    db_session.commit()

    telemetry_query = select(Telemetry)
    telemetry_items = db_session.exec(telemetry_query).all()

    assert len(telemetry_items) == 1
    returned_item1 = telemetry_items[0]
    assert returned_item1.id == id
    assert returned_item1.type_ == 1
    assert returned_item1.value is None

    # Start the delete portion of the test
    db_session.delete(main_telemetry)
    main_telemetry_items = db_session.exec(main_telemetry_query).all()
    assert len(main_telemetry_items) == 0

    # Make sure it cascades
    commands_items = db_session.exec(telemetry_query).all()
    assert len(commands_items) == 0


def test_comms_session_basic(db_session: Session, default_start_time: datetime, default_comms_session: CommsSession):
    # Setup
    id = default_comms_session.id  # sqlmodel generates the uuid before it's sent to the db

    # Db actions
    db_session.add(default_comms_session)
    db_session.commit()
    comms_session_query = select(CommsSession)
    comms_session_items = db_session.exec(comms_session_query).all()

    # Assertions
    assert len(comms_session_items) == 1
    returned_item1 = comms_session_items[0]
    assert returned_item1.id == id
    assert returned_item1.start_time == default_start_time
    assert returned_item1.end_time is None
    assert returned_item1.status == SessionStatus.PENDING


def test_packet_basic(db_session: Session, default_comms_session: CommsSession):
    # Setup the comms session table
    db_session.add(default_comms_session)
    db_session.commit()
    comms_session_query = select(CommsSession)
    comms_session_items = db_session.exec(comms_session_query).all()
    assert len(comms_session_items) == 1  # Make sure it was inserted into the db
    id = comms_session_items[0].id

    # Test the packet
    # TODO: Move this data to a fixture
    packet = Packet(
        session_id=id,
        raw_data=b"Hello world",
        type_=MainPacketType.UPLINK,
        payload_data=b"Hello world. Extra info.",
        offset=1,
    )
    db_session.add(packet)
    db_session.commit()
    packet_query = select(Packet)
    packet_items = db_session.exec(packet_query).all()

    assert len(packet_items) == 1
    returned_items1 = packet_items[0]
    assert returned_items1.type_ == MainPacketType.UPLINK
    assert returned_items1.raw_data == b"Hello world"
    assert returned_items1.payload_data == b"Hello world. Extra info."
    assert returned_items1.offset == 1

    # Test the delete cascading
    db_session.delete(default_comms_session)
    db_session.commit()
    comms_session_items = db_session.exec(comms_session_query).all()
    assert len(comms_session_items) == 0

    packet_items = db_session.exec(packet_query).all()
    assert len(packet_items) == 0
