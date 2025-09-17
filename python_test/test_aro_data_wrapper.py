# python_test/test_aro_data_wrapper.py
from datetime import datetime, timedelta
from decimal import Decimal
from uuid import UUID

import pytest

# wrappers (for monkeypatch targets)
from gs.backend.data.data_wrappers.aro_wrapper import (
    aro_request_wrapper as req_wrapper,
)
from gs.backend.data.data_wrappers.aro_wrapper import (
    aro_user_auth_token_wrapper as tok_wrapper,
)
from gs.backend.data.data_wrappers.aro_wrapper import (
    aro_user_data_wrapper as user_wrapper,
)
from gs.backend.data.data_wrappers.aro_wrapper import (
    aro_user_login_wrapper as login_wrapper,
)

# funcs under test
from gs.backend.data.data_wrappers.aro_wrapper.aro_request_wrapper import (
    add_request,
    get_all_requests,
)
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_auth_token_wrapper import (
    add_auth_token,
    get_all_auth_tokens,
)
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper import add_user
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_login_wrapper import add_login
from gs.backend.data.enums.aro_auth_token import AROAuthToken
from gs.backend.data.enums.aro_requests import ARORequestStatus

# ensure models are imported/registered
from gs.backend.data.tables import aro_user_tables as _u  # noqa: F401
from gs.backend.data.tables import transactional_tables as _t

# schema metadatas
from gs.backend.data.tables.aro_user_tables import ARO_USER_SCHEMA_METADATA
from gs.backend.data.tables.transactional_tables import TRANSACTIONAL_SCHEMA_METADATA
from sqlalchemy.pool import StaticPool
from sqlmodel import Session, create_engine


@pytest.fixture(name="session")
def session_fixture():
    engine = create_engine(
        "sqlite+pysqlite:///:memory:",
        connect_args={"check_same_thread": False},
        poolclass=StaticPool,
    )

    with engine.begin() as conn:
        conn.exec_driver_sql("ATTACH DATABASE ':memory:' AS aro_users;")
        conn.exec_driver_sql("ATTACH DATABASE ':memory:' AS transactional;")
        # create schema tables only (no global SQLModel.metadata)
        ARO_USER_SCHEMA_METADATA.create_all(conn)
        TRANSACTIONAL_SCHEMA_METADATA.create_all(conn)

    with Session(engine) as s:
        yield s


@pytest.fixture(autouse=True)
def override_get_db_session(monkeypatch, session):
    engine = session.get_bind()

    def _get_session():
        return Session(engine)

    monkeypatch.setattr(req_wrapper, "get_db_session", _get_session, raising=True)
    monkeypatch.setattr(tok_wrapper, "get_db_session", _get_session, raising=True)
    monkeypatch.setattr(user_wrapper, "get_db_session", _get_session, raising=True)
    monkeypatch.setattr(login_wrapper, "get_db_session", _get_session, raising=True)


def test_user_creation():
    user = add_user(
        call_sign="KEVWAN",
        email="kevian@gmail.com",
        f_name="kevin",
        l_name="wan",
        phone_number="8888888888",
    )
    assert user.email == "kevian@gmail.com"
    assert user.call_sign == "KEVWAN"
    assert user.first_name == "kevin"
    assert user.last_name == "wan"
    assert user.phone_number == "8888888888"


def test_login_creation():
    user = add_user(
        call_sign="BEVWAN",
        email="bevian@gmail.com",
        f_name="bevin",
        l_name="ban",
        phone_number="9999999999",
    )
    login = add_login(
        email=user.email,
        pwd="password",
        hash_algo="kevalgo",
        user_data_id=user.id,
        email_verification_token="SIXSEVEN_*hits_the_thing*",
    )
    assert login.email == "bevian@gmail.com"
    assert login.password == "password"
    assert login.hashing_algorithm_name == "kevalgo"
    assert login.user_data_id == user.id
    assert login.email_verification_token == "SIXSEVEN_*hits_the_thing*"


def test_auth_token_creation():
    user = add_user(
        call_sign="TOKUSR",
        email="tokuser@example.com",
        f_name="tok",
        l_name="user",
        phone_number="1112223333",
    )
    before = {t.id for t in get_all_auth_tokens()}
    expiry = datetime.now() + timedelta(hours=2)
    token = add_auth_token(
        token="tok123",
        user_data_id=user.id,
        expiry=expiry,
        auth_type=AROAuthToken.DUMMY,
    )
    after = {t.id for t in get_all_auth_tokens()}
    assert token.token == "tok123"
    assert isinstance(token.user_data_id, UUID)
    assert token.user_data_id == user.id
    assert isinstance(token.expiry, datetime)
    assert token.auth_type == AROAuthToken.DUMMY
    assert token.id in after
    assert token.id not in before


def test_request_creation():
    created_on = datetime.now()
    request_sent_obc = created_on + timedelta(minutes=1)
    taken_date = created_on + timedelta(minutes=2)
    transmission = created_on + timedelta(minutes=3)
    before = {r.id for r in get_all_requests()}
    req = add_request(
        long=Decimal("123.456"),
        lat=Decimal("49.282"),
        created_on=created_on,
        request_sent_obc=request_sent_obc,
        taken_date=taken_date,
        transmission=transmission,
        status=ARORequestStatus.PENDING,
    )
    after = {r.id for r in get_all_requests()}
    assert req.longitude == Decimal("123.456")
    assert req.latitude == Decimal("49.282")
    assert abs((req.created_on - created_on).total_seconds()) < 1
    assert abs((req.request_sent_to_obc_on - request_sent_obc).total_seconds()) < 1
    assert req.status == ARORequestStatus.PENDING
    assert req.id in after
    assert req.id not in before
