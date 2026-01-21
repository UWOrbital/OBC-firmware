from abc import ABC
from typing import Any, Generic, TypeVar
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.base_model import BaseSQLModel

T = TypeVar("T", bound=BaseSQLModel)
PK = TypeVar("PK", int, UUID)


class AbstractWrapper(ABC, Generic[T, PK]):
    """
    An Abstract Base Class for all data wrappers.
    """

    model: type[T]

    async def get_all(self) -> list[T]:
        """
        Get all data wrapper for the unspecified model

        :return: a list of all model instances
        """
        async with get_db_session() as session:
            result = await session.exec(select(self.model))
            return list(result.all())

    async def get_by_id(self, obj_id: PK) -> T:
        """
        Retrieve data wrapper for the unspecified model

        :param obj_id: PK of the model instance to be retrieved
        :return: the retrieved instance
        """
        async with get_db_session() as session:
            obj = await session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            return obj

    async def create(self, data: dict[str, Any]) -> T:
        """
        Post data wrapper for the unspecified model

        :param data: the JSON object of the model instance to be created
        :return: the newly created instance
        """
        async with get_db_session() as session:
            obj = self.model(**data)
            session.add(obj)
            await session.commit()
            await session.refresh(obj)
            return obj

    async def delete_by_id(self, obj_id: PK) -> T:
        """
        Delete data wrapper for the unspecified model

        :param obj_id: PK of the model instance to be deleted
        :return: the deleted instance
        """
        async with get_db_session() as session:
            obj = await session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            session.delete(obj)
            await session.commit()
            return obj
