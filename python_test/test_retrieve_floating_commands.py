from datetime import datetime
from uuid import uuid4

from gs.backend.data.data_wrappers.wrappers import (
    CommandsWrapper,
    CommsSessionWrapper,
    MainCommandWrapper,
    PacketCommandsWrapper,
    PacketWrapper,
)
from gs.backend.data.enums.transactional import MainPacketType


async def test_retrieve_floating_commands_filters(db_session):
    from gs.backend.data.tables.main_tables import MainCommand

    pcw = PacketCommandsWrapper()
    cw = CommandsWrapper()
    mc = MainCommandWrapper()
    pw = PacketWrapper()
    csw = CommsSessionWrapper()

    # Create main command directly to avoid ID conflicts
    main_cmd = MainCommand(id=101, name="test", data_size=1, total_size=1)
    db_session.add(main_cmd)
    db_session.flush()
    cmd_type = main_cmd.id

    packet_id = uuid4()

    comms_session = await csw.create({"id": uuid4(), "start_time": datetime.now()})
    packet = await pw.create(
        dict(
            id=packet_id,
            session_id=comms_session.id,
            raw_data=b"\x00",
            type_=MainPacketType.UPLINK,
            payload_data=b"\x00",
            offset=0,
        )
    )

    cmd_in_packet = await cw.create(dict(id=uuid4(), type_=cmd_type))
    cmd_free = await cw.create(dict(id=uuid4(), type_=cmd_type))
    cmd_free2 = await cw.create(dict(id=uuid4(), type_=cmd_type))

    await pcw.create(dict(packet_id=packet.id, command_id=cmd_in_packet.id))

    result = await cw.retrieve_floating_commands()
    for command in result:
        assert command.id in [cmd_free.id, cmd_free2.id]


async def test_retrieve_floating_commands_no_packet_commands(db_session):
    from gs.backend.data.tables.main_tables import MainCommand

    cw = CommandsWrapper()
    mc = MainCommandWrapper()

    # Create main command directly to avoid ID conflicts
    main_cmd = MainCommand(id=102, name="test", data_size=1, total_size=1)
    db_session.add(main_cmd)
    db_session.flush()
    cmd_type = main_cmd.id

    await cw.create(dict(id=uuid4(), type_=cmd_type))
    await cw.create(dict(id=uuid4(), type_=cmd_type))

    result = await cw.retrieve_floating_commands()
    expected = await cw.get_all()
    assert {c.id for c in result} == {c.id for c in expected}
