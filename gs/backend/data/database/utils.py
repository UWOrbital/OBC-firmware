def to_foreign_key_value(
    foreign_table_name: str,
    foreign_schema_name: str | None = None,
    foreign_column_name: str = "id",
) -> str:
    """
    @brief Generates the foreign key column value to be used in foreign key constraints

    @param foreign_table_name (str) - The table to which to create a reference to.
    @param foreign_schema_name (str | None) - The schema for which to link to or the current schema if not provided
    @param foreign_column_name (str) - The column to reference to. The default is "id"

    @return The foreign key value
    """
    schema_portion = f"{foreign_schema_name}." if foreign_schema_name is not None else ""
    foreign_key_value = f"{schema_portion}{foreign_table_name}.{foreign_column_name}"
    return foreign_key_value
