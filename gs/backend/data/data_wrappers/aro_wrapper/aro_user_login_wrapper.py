from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.aro_user_tables import AROUserLogin


def get_all_logins() -> list[AROUserLogin]:
    """
    @brief gets all the logins
    """
    with get_db_session() as session:
        user_logins = list(session.exec(select(AROUserLogin)).all())
        return user_logins


def add_login(email: str, pwd: str, hash_algo: str, user_data_id: str, email_verification_token: str) -> AROUserLogin:
    """
    @brief add a new user login
    """
    with get_db_session() as session:
        # check if the user exists already
        user_login = session.exec(select(AROUserLogin).where(AROUserLogin.email == email))

        if user_login:
            print("User login already exists")
            return user_login

        user_login = AROUserLogin(
            email=email,
            password=pwd,
            hashing_algorithm_name=hash_algo,
            user_data_id=user_data_id,
            email_verification_token=email_verification_token,
        )

        session.add(user_login)
        session.commit()
        session.resfresh(user_login)
        return user_login
