# TODO:(335) Improve loading the configuration
from os import environ

from dotenv import load_dotenv

load_dotenv()


# TODO: Make these throw an exception if they are None
GS_DATABASE_USER = environ.get("GS_DATABASE_USER")
GS_DATABASE_PASSWORD = environ.get("GS_DATABASE_PASSWORD")
GS_DATABASE_LOCATION = environ.get("GS_DATABASE_LOCATION")
GS_DATABASE_PORT = environ.get("GS_DATABASE_PORT")
GS_DATABASE_NAME = environ.get("GS_DATABASE_NAME")
