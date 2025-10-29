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
    """
    Data wrapper for AROUsers table.
    """

    model = AROUsers


class AROUserAuthTokenWrapper(AbstractWrapper[AROUserAuthToken]):
    """
    Data wrapper for AROUserAuthToken table.
    """

    model = AROUserAuthToken


class AROUserLoginWrapper(AbstractWrapper[AROUserLogin]):
    """
    Data wrapper for AROUserLogin table.
    """

    model = AROUserLogin


class ARORequestWrapper(AbstractWrapper[ARORequest]):
    """
    Data wrapper for ARORequest table.
    """

    model = ARORequest


class MainCommandWrapper(AbstractWrapper[MainCommand]):
    """
    Data wrapper for MainCommand table.
    """

    model = MainCommand


class MainTelemetryWrapper(AbstractWrapper[MainTelemetry]):
    """
    Data wrapper for MainTelemetry table.
    """

    model = MainTelemetry


class CommsSessionWrapper(AbstractWrapper[CommsSession]):
    """
    Data wrapper for CommsSession table.
    """

    model = CommsSession


class PacketWrapper(AbstractWrapper[Packet]):
    """
    Data wrapper for Packet table.
    """

    model = Packet


class PacketCommandsWrapper(AbstractWrapper[PacketCommands]):
    """
    Data wrapper for PacketCommands table.
    """

    model = PacketCommands


class PacketTelemetryWrapper(AbstractWrapper[PacketTelemetry]):
    """
    Data wrapper for PacketTelemetry table.
    """

    model = PacketTelemetry


class CommandsWrapper(AbstractWrapper[Commands]):
    """
    Data wrapper for Commands table.
    """

    model = Commands

    def retrieve_floating_commands(self) -> list[Commands]:
        """
        Retrieves all commands which do not have a valid entry in
        the packet_commands table.
        A command which is not valid is considered as any command whose ID
        does not match with any command_id in the packet_commands table
        """
        packet_commands = PacketCommandsWrapper().get_all()
        packet_ids = [packet_command.command_id for packet_command in packet_commands]

        commands = self.get_all()
        floating_commands = [fc for fc in commands if fc.id not in packet_ids]

        return floating_commands


class TelemetryWrapper(AbstractWrapper[Telemetry]):
    """
    Data wrapper for Telemetry table.
    """

    model = Telemetry
