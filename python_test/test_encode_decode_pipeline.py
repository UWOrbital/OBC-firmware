import os
from pathlib import Path

import pytest
from ax25 import FrameType
from interfaces.obc_gs_interface.ax25 import ax25_gs as ax25

# TEST: Decoding basic data with bit flips
# TEST: Encoding and sending data


def test_ax25_recieve():
    path = (Path(__file__).parent / "../build/test/test_interfaces/unit/encode.bin").resolve()
    file = path.open("rb")
    bin = file.read()
    ax25_proto = ax25.AX25("ATLAS", "AKITO")
    rcv_frame = ax25_proto.decode_frame(bin)
    assert str(rcv_frame.src) == "ATLAS"
    assert str(rcv_frame.dst) == "AKITO"
    assert str(rcv_frame.control.frame_type) == "FrameType.I"
    file.close


def test_ax25_send():
    path = (Path(__file__).parent / "../build/test/test_interfaces/unit/encode_more.bin").resolve()
    file = path.open("wb")
    file.truncate(0)
    ax25_proto = ax25.AX25("ATLAS", "AKITO")
    send_frame = ax25_proto.encode_frame(b"A" * 255, FrameType.I, 0)
    recv_frame = ax25_proto.decode_frame(send_frame)
    file.write(send_frame)
    file.close
