import os
import smtplib
from email.message import EmailMessage
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()

def require_env(name: str) -> any:
    value = os.getenv(name)
    if not value:
        raise RuntimeError(f"Missing required environment variable: {name}")
    return value

# Set up in http://smtp2go.com/
sender_email: str = require_env("sender_email")
smtp_user: str = require_env("smtp_user")
smtp_password: str = require_env("smtp_password")
smtp_host: str = require_env("smtp_host")
smtp_port: int = require_env("smtp_port")

def send_verification_email(recipient: str, first_name: str) -> None:
    """
    Send a plain-text verification email to a single recipient via SMTP.

    :param recipient: Recipient's email address
    :param first_name: Recipient's first name
    :returns: None
    """
    msg = EmailMessage()
    msg["Subject"] = "Verify Your ARO Sign Up"
    msg["From"] = sender_email
    msg["To"] = recipient
    msg.set_content(
        f"""Hi {first_name},

        Thanks for signing up for the ARO map.
        To verify your account, please reply to this email with:
        - Call sign
        - Last name
        - City
        - Province
        - Postal code
        
        Once we review your reply, we’ll enable your login so you can access ARO features.
        
        If you didn’t request this, please ignore this message.
        
        UW Orbital""")

    with smtplib.SMTP(smtp_host, smtp_port) as server:
        server.ehlo()
        server.starttls()
        server.ehlo()
        server.login(smtp_user, smtp_password)
        server.send_message(msg)