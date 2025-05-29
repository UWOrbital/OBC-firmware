import pytest
from ax25 import FrameType
from hypothesis import example, given, settings
from hypothesis.strategies import binary
from interfaces import RS_ENCODED_DATA_SIZE
from interfaces.obc_gs_interface.ax25 import AX25


@given(
    binary(min_size=RS_ENCODED_DATA_SIZE, max_size=RS_ENCODED_DATA_SIZE).map(
        lambda b: b.replace(b"\x7e", b"").ljust(RS_ENCODED_DATA_SIZE, b"\x00")
    )
)
def test_I_frame_encode_decode(data_bytes: bytes):
    framer = AX25("AKITO", "ATLAS")
    frame = framer.encode_frame(data_bytes, FrameType.I)
    stuffed_frame = framer.stuff(frame)
    unstuffed_frame = framer.unstuff(stuffed_frame)
    frame_decoded = b""
    frame_data = b""
    try:
        frame_decoded = framer.decode_frame(unstuffed_frame)
        frame_data = bytes(frame_decoded.data)
    except ValueError:
        print("--- FRAME DATA ---")
        print(frame)
        print("--- STUFFED FRAME ---")
        print(stuffed_frame)
        print("--- UNSTUFFED FRAME ---")
        print(unstuffed_frame)
        print("--- FRAME DATA ---")
        print(frame_data)

    assert frame_data == data_bytes


# For the acknowledges that we send to the OBC
def test_U_frame_encode_decode():
    framer = AX25("AKITO", "ATLAS")
    frame = framer.encode_frame(None, FrameType.SABM, 0, True)
    frame_stuffed = framer.stuff(frame)
    frame_unstuffed = framer.unstuff(frame_stuffed)
    frame_decoded = framer.decode_frame(frame_unstuffed)

    assert frame_decoded.control.frame_type == FrameType.SABM
    assert frame_decoded.pid == 0
    assert frame_decoded.data == None
    assert frame_decoded.src.call == "AKITO"
    assert frame_decoded.src.ssid == 0
    assert frame_decoded.dst.call == "ATLAS"
    assert frame_decoded.dst.ssid == 0
