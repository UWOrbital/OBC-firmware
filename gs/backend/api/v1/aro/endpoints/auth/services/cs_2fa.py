from fastapi import HTTPException, status
from gs.backend.data.data_wrappers.wrappers import AROUsersWrapper
from gs.backend.data.tables.aro_user_tables import AROUsers

def callsign_verified(call_sign: str) -> bool:
    """
    callsign_verified 的 Docstring
    
    TODO: Checks call_sign against the government CSV file.
    """
    # Future: Query against AROUserCallsigns
    return False

def verify_user_callsign(callsign: str, user: AROUsers) -> AROUsers:
    if not callsign_verified(callsign):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Callsign unable to be verified."
        )

    users = AROUsersWrapper()
    updated_user = users.update(user.id, {
        "callsign" : callsign,
        "is_callsign_verified" : True,
    })

    return updated_user