from uuid import uuid4

from gs.backend.data.enums.transactional import CommandStatus
from gs.backend.data.tables.main import MainCommand, MainTelemetry
from gs.backend.data.tables.transactional import Commands, Telemetry
from sqlmodel import Session, select


def test_commands_basic(db_session: Session):
    main_command1 = MainCommand(id=1, name="Test 1", data_size=1, total_size=2, format="int 7 bytes", params="time")
    db_session.add(main_command1)
    db_session.commit()

    id = uuid4()
    command1 = Commands(id=id, type_=main_command1.id, params="1234567")
    db_session.add(command1)
    db_session.commit()

    query = select(Commands)
    items = db_session.exec(query).all()

    assert len(items) == 1
    returned_item1 = items[0]
    assert returned_item1.id == id
    assert returned_item1.type_ == 1
    assert returned_item1.params == "1234567"
    assert returned_item1.status == CommandStatus.PENDING


def test_telemetry_basic(db_session: Session):
    main_telemetry = MainTelemetry(id=1, name="Test 1", data_size=1, total_size=2, format="int 7 bytes")
    db_session.add(main_telemetry)
    db_session.commit()

    id = uuid4()
    telemetry1 = Telemetry(id=id, type_=main_telemetry.id)
    db_session.add(telemetry1)
    db_session.commit()

    query = select(Telemetry)
    items = db_session.exec(query).all()

    assert len(items) == 1
    returned_item1 = items[0]
    assert returned_item1.id == id
    assert returned_item1.type_ == 1
    assert returned_item1.value is None
