from gs.backend.data.data_wrappers.abstract_wrapper import AbstractWrapper  # SEE abstract_wrapper.py FOR LOGIC
from gs.backend.data.database.engine import get_db_session
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

    def get_by_id(self, obj_id: int) -> MainCommand:
        """
        Retrieve data wrapper for MainCommand

        :param obj_id: int of the model instance to be retrieved
        :return: the retrieved instance
        """
        with get_db_session() as session:
            obj = session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            return obj

    def delete_by_id(self, obj_id: int) -> MainCommand:
        """
        Delete data wrapper for MainCommand

        :param obj_id: int of the model instance to be deleted
        :return: the deleted instance
        """
        with get_db_session() as session:
            obj = session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            session.delete(obj)
            session.commit()
            return obj


class MainTelemetryWrapper(AbstractWrapper[MainTelemetry]):
    """
    Data wrapper for MainTelemetry table.
    """

    model = MainTelemetry

    def get_by_id(self, obj_id: int) -> MainTelemetry:
        """
        Retrieve data wrapper for MainTelemetry

        :param obj_id: int of the model instance to be retrieved
        :return: the retrieved instance
        """
        with get_db_session() as session:
            obj = session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            return obj

    def delete_by_id(self, obj_id: int) -> MainTelemetry:
        """
        Delete data wrapper for MainTelemetry

        :param obj_id: int of the model instance to be deleted
        :return: the deleted instance
        """
        with get_db_session() as session:
            obj = session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            session.delete(obj)
            session.commit()
            return obj


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


class TelemetryWrapper(AbstractWrapper[Telemetry]):
    """
    Data wrapper for Telemetry table.
    """

    model = Telemetry
