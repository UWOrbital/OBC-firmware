from abc import ABC
from typing import Any, Generic, TypeVar
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.base_model import BaseSQLModel

T = TypeVar("T", bound=BaseSQLModel)


class AbstractWrapper(ABC, Generic[T]):
    """ """

    model: type[T]

    def get_all(self) -> list[T]:
        """ """
        with get_db_session() as session:
            return list(session.exec(select(self.model)).all())

    def create(self, data: dict[str, Any]) -> T:
        """ """
        with get_db_session() as session:
            obj = self.model(**data)
            session.add(obj)
            session.commit()
            session.refresh(obj)
            return obj

    def delete_by_id(self, obj_id: int | UUID) -> T:
        """ """
        with get_db_session() as session:
            obj = session.get(self.model, obj_id)
            if not obj:
                raise ValueError(f"{self.model.__name__} with ID {obj_id} not found.")
            session.delete(obj)
            session.commit()
            return obj
