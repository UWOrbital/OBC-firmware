import sys
from typing import Final

from loguru import logger as logger

DEFAULT_LOG_FORMAT: Final[str] = """<green>{time:YYYY-MM-DD at HH:mm:ss.SSSSZ}</green> | \
<level>{level: <8}</level> | \
<cyan>{name}</cyan>:<cyan>{function}</cyan>:<cyan>{line}</cyan> \
- <level>{message}</level> \
"""


def logger_setup(*, enqueue: bool = False, diagnose: bool = True) -> None:
    """
    Set up the logger and return it.
    The logger will log everything to a file, info to stdout, and warnings and above to stderr.
    @param enqueue - Whether to enqueue messages for asynchronous processing.
    @param diagnose - Whether to enable diagnostic mode.
    """
    # Remove any existing sinks
    logger.remove()

    # Log everything to a file
    logger_setup_file(enqueue=enqueue, diagnose=diagnose)

    # Log info to stdout
    logger.add(
        sys.stdout,
        filter=lambda record: record["level"].name == "INFO",
        format=DEFAULT_LOG_FORMAT,
        level="INFO",
        colorize=True,
        enqueue=enqueue,
        diagnose=diagnose,
    )

    # Log warnings and above to stderr
    logger.add(
        sys.stderr,
        format=DEFAULT_LOG_FORMAT,
        level="WARNING",
        colorize=True,
        enqueue=enqueue,
        diagnose=diagnose,
    )


def logger_setup_file(*, enqueue: bool = False, diagnose: bool = True) -> None:
    """Set up the logger to log everything to a file."""
    logger.add(
        "gs_python.log",
        serialize=True,
        format=DEFAULT_LOG_FORMAT,
        rotation="1 week",
        retention="1 month",
        enqueue=enqueue,
        diagnose=diagnose,
    )


async def logger_close() -> None:
    """
    Close the logger for the async applications.
    If the logger was setup using `enqueue=True`, this function should be awaited before the application exits.
    """
    await logger.complete()
    logger.remove()  # Clear existing sinks to prevent semaphore leakage
