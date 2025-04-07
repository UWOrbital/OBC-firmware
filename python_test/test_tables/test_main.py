from gs.backend.data.tables.main import MainCommand, MainTelemetry
from sqlmodel import Session, select


def test_main_command_basic(db_session: Session):
    test1 = MainCommand(id=1, name="Test 1", data_size=1, total_size=2, format="int 7 bytes", params="time")
    db_session.add(test1)
    db_session.commit()
    query = select(MainCommand)
    items = db_session.exec(query).all()
    assert len(items) == 1
    returned_item1 = items[0]
    assert returned_item1.id == 1
    assert returned_item1.data_size == 1
    assert returned_item1.total_size == 2
    assert returned_item1.name == "Test 1"
    assert returned_item1.format == "int 7 bytes"
    assert returned_item1.params == "time"


def test_main_telemetry_basic(db_session: Session):
    test1 = MainTelemetry(id=1, name="Test 1", data_size=1, total_size=2, format="int 7 bytes")
    db_session.add(test1)
    db_session.commit()
    query = select(MainTelemetry)
    items = db_session.exec(query).all()
    assert len(items) == 1
    returned_item1 = items[0]
    assert returned_item1.id == 1
    assert returned_item1.data_size == 1
    assert returned_item1.total_size == 2
    assert returned_item1.name == "Test 1"
    assert returned_item1.format == "int 7 bytes"
