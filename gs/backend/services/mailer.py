import os
import smtplib
from email.message import EmailMessage
from dotenv import load_dotenv

smtp_host: str = "mail.smtp2go.com"
smtp_port: int = 587

# Set up in http://smtp2go.com/
# Load environment variables from .env file
load_dotenv()
sender_email: str = os.getenv("sender_email")
smtp_user: str = os.getenv("smtp_user")
smtp_password: str = os.getenv("smtp_password")

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