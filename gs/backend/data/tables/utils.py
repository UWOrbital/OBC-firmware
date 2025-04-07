from typing import Any, TypeVar

from sqlalchemy import Column, ForeignKey

T = TypeVar("T")


def foreign_key_column(
    referenced_table: str,
    column_type: type[T],  # this column is used to determine the type of Column used
    referenced_column: str = "id",
    schema_name: str | None = None,
    **kwargs: Any,  # noqa: ANN401
) -> Column[T]:
    """
    @brief Creates a foreign key column with support for schemas.

    @param referenced_table (str) - The name of the referenced table.
    @param referenced_column (str) - The name of the referenced column.
    @param schema_name (str) - The schema of the referenced table.
    @param column_type (type[T]) - The type of the column.
    @param kwargs (Any) - Additional arguments to pass to the Column constructor.
    @return A Column object with the specified foreign key.
    """
    schema_portion = schema_name + "." if schema_name else ""
    return Column(ForeignKey(f"{schema_portion}{referenced_table}.{referenced_column}"), **kwargs)
