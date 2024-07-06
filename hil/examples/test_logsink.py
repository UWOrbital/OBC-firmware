import os
import sys
import time

import log_module
import pytest

MIN_LOGS = 5
SPECIFIC_LOG = "Temperature: 0.000000"
MAX_TIME = 10

# scoping custom c++ wrapper module in path
build_dir = os.path.join(os.path.dirname(__file__), "..", "..", "build_hil", "hil")
sys.path.insert(0, build_dir)


@pytest.fixture(scope="session")
def log_file_instance() -> None:
    """
    @brief setup log_file_instance. for tests to generate file
    """
    filename = "logsink.txt"
    logger = log_module.LogSink("/dev/ttyS0", 115200, filename)
    logger.start()
    time.sleep(5)
    logger.stop()
    yield filename
    os.remove(filename)


def test_log_generation(log_file_instance: str) -> None:
    """
    @brief checks to see if the file can be read
    @param generated file from log_file_instance test fixture
    """
    with open(log_file_instance) as f:
        assert f.readable()


def test_num_lines(log_file_instance: str) -> None:
    """
    @brief checks for log count in file
    @param generated file from log_file_instance test fixture
    """
    with open(log_file_instance) as f:
        c = 0
        for x in f:
            if x != "":
                c += 1
        assert c >= MIN_LOGS


def test_specific_logs(log_file_instance: str) -> None:
    """
    @brief checks for specific generated log in file
    @param generated file from log_file_instance test fixture
    """
    found = False
    with open(log_file_instance) as f:
        for x in f:
            x = x[15:]
            if x == SPECIFIC_LOG:
                found = True
                break
    assert found is True


def test_timely_logs(log_file_instance: str) -> None:
    """
    @brief checks for logs being generated in specific time intervals
    @param generated file from log_file_instance test fixture
    """
    prevtime = 100
    with open(log_file_instance) as f:
        for x in f:
            assert (int(x[7:9]) - prevtime) < MAX_TIME
