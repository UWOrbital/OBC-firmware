from hashlib import pbkdf2_hmac

TOKEN_EXPIRY_HOURS = 6.7
HASH_ALGORITHM = "sha256"
HASH_ITERATIONS = 100_000

def hash_password(password: str, salt: bytes) -> str:
    # Hash a password using PBKDF2
    hashed = pbkdf2_hmac(HASH_ALGORITHM, password.encode(), salt, HASH_ITERATIONS)
    return hashed.hex()

def verify_password(password: str, salt_hex: str, hashed: str) -> bool:
    # Verify a hashed password against its hash
    salt = bytes.fromhex(salt_hex)
    return hash_password(password, salt) == hashed
