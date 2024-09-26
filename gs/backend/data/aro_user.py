from sqlmodel import Field, SQLModel


class AroUser(SQLModel, table=True):
    """Aro user's data model"""

    id: int | None = Field(default=None, primary_key=True)
    call_sign: str = Field(min_length=5, max_length=6)
    first_name: str
    last_name: str
    phone_number: str = Field(max_length=17)  # Max digits is 17
