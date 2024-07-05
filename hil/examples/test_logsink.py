import os
import sys
import time

import pytest

minLogs = 5
specificLog = "Temperature: 0.000000"
maxTime = 10

# scoping custom c++ wrapper module in path
build_dir = os.path.join(os.path.dirname(__file__), "..", "..", "build_hil", "hil")
sys.path.insert(0, build_dir)
import log_module


@pytest.fixture(scope="session")
def instance():
    filename = "logsink.txt"
    logger = log_module.LogSink("/dev/ttyS0", 115200, filename)
    logger.start()
    time.sleep(5)
    logger.stop()
    yield filename
    os.remove(filename)


def test_logGeneration(instance):  # checks to see if the file can be read
    f = open(instance)
    assert f.readable() == True
    f.close()


def test_numLines(instance):  # checks for log count
    f = open(instance)
    c = 0
    for x in f:
        c += 1
    assert c >= minLogs
    f.close()


def test_specificLogs(instance):  # checks for specific log, currently intended to fail
    f = open(instance)
    found = False
    for x in f:
        x = x[15:]
        if x == specificLog:
            found = True
            break
    assert found == True
    f.close()


def test_timelyLogs(instance):  # checks for logs being generated in specific times
    f = open(instance)
    prevtime = 100
    for x in f:
        assert (int(x[7:9]) - prevtime) < maxTime
    f.close()
