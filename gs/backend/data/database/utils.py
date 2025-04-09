from typing import Any

from sqlalchemy import ForeignKeyConstraint


def foreign_key_constraint(
    *,
    column_name: Any,  # noqa: ANN401
    foreign_table_name: str,
    foreign_schema_name: str,
    foreign_column_name: str = "id",
    onupdate: str = "CASCADE",
    ondelete: str = "CASCADE",
    **kwargs: Any,  # noqa: ANN401
) -> ForeignKeyConstraint:
    """
    @brief Creates the foreign key constraint
    """
    foreign_key_value = f"{foreign_schema_name}.{foreign_table_name}.{foreign_column_name}"
    constraint = ForeignKeyConstraint(
        [column_name], [foreign_key_value], ondelete=ondelete, onupdate=onupdate, **kwargs
    )
    return constraint


def to_foreign_key_value(
    foreign_table_name: str,
    foreign_schema_name: str,
    foreign_column_name: str = "id",
) -> str:
    """
    @brief Generates the foreign key column value to be used in foreign key constraints
    """
    foreign_key_value = f"{foreign_schema_name}.{foreign_table_name}.{foreign_column_name}"
    return foreign_key_value
