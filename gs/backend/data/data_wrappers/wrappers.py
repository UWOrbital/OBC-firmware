from gs.backend.data.data_wrappers.abstract_wrapper import AbstractWrapper  # SEE abstract_wrapper.py FOR LOGIC
from gs.backend.data.tables.aro_user_tables import AROUserAuthToken, AROUserLogin, AROUsers
from gs.backend.data.tables.main_tables import MainCommand, MainTelemetry
from gs.backend.data.tables.transactional_tables import (
    ARORequest,
    Commands,
    CommsSession,
    Packet,
    PacketCommands,
    PacketTelemetry,
    Telemetry,
)


class AROUsersWrapper(AbstractWrapper[AROUsers]):
    """"""

    model = AROUsers


class AROUserAuthTokenWrapper(AbstractWrapper[AROUserAuthToken]):
    """"""

    model = AROUserAuthToken


class AROUserLoginWrapper(AbstractWrapper[AROUserLogin]):
    """"""

    model = AROUserLogin


class ARORequestWrapper(AbstractWrapper[ARORequest]):
    """"""

    model = ARORequest


class MainCommandWrapper(AbstractWrapper[MainCommand]):
    """"""

    model = MainCommand


class MainTelemetryWrapper(AbstractWrapper[MainTelemetry]):
    """"""

    model = MainTelemetry


class CommsSessionWrapper(AbstractWrapper[CommsSession]):
    """"""

    model = CommsSession


class PacketWrapper(AbstractWrapper[Packet]):
    """"""

    model = Packet


class PacketCommandsWrapper(AbstractWrapper[PacketCommands]):
    """"""

    model = PacketCommands


class PacketTelemetryWrapper(AbstractWrapper[PacketTelemetry]):
    """"""

    model = PacketTelemetry


class CommandsWrapper(AbstractWrapper[Commands]):
    """"""

    model = Commands


class TelemetryWrapper(AbstractWrapper[Telemetry]):
    """"""

    model = Telemetry
