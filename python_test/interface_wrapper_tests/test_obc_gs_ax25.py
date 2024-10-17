import interfaces.obc_gs_interface.python_wrappers.error_codes.obc_gs_errors as errors
import interfaces.obc_gs_interface.python_wrappers.rs_encode_decode.obc_gs_fec as fec
import interfaces.obc_gs_interface.python_wrappers.uplink_flow_encode.obc_gs_ax25 as ax25
import pytest


@pytest.fixture
def telem_data():
    """Returns a valid packed telemetry data bytearray."""
    return bytearray(fec.RS_ENCODED_SIZE)


@pytest.fixture
def unstuffed_ax25_data():
    """Returns a valid unstuffed AX25 I-frame as a dictionary."""
    return {"data": bytearray(ax25.AX25_MINIMUM_I_FRAME_LEN), "length": 0}


@pytest.fixture
def packed_ax25_u_frame():
    """Returns a valid packed AX25 U-frame as a dictionary."""
    return {"data": bytearray(ax25.AX25_MAXIMUM_U_FRAME_CMD_LENGTH), "length": 0}


@pytest.fixture
def cubsat_call_sign():
    """Returns a valid packed AX25 U-frame as a dictionary."""
    return {"data": bytearray(ax25.AX25_DEST_ADDR_BYTES), "length": ax25.AX25_DEST_ADDR_BYTES}


@pytest.fixture
def groundstation_call_sign():
    """Returns a valid packed AX25 U-frame as a dictionary."""
    return {"data": bytearray(ax25.AX25_DEST_ADDR_BYTES), "length": ax25.AX25_DEST_ADDR_BYTES}


def test_i_frame_little_stuff(telem_data, unstuffed_ax25_data, groundstation_call_sign):
    """Test sending and receiving an I-frame with little bit stuffing."""
    ax25.setCurrentLinkDestAddress(groundstation_call_sign)
    result = ax25.ax25SendIFrame(telem_data, len(telem_data), unstuffed_ax25_data)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS

    packed_ax25_data = {"data": bytearray(ax25.AX25_MAXIMUM_PKT_LEN), "length": 0}
    result = ax25.ax25Stuff(
        unstuffed_ax25_data["data"], unstuffed_ax25_data["length"], packed_ax25_data["data"], packed_ax25_data["length"]
    )
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS

    unstuffed_packet = {"data": bytearray(ax25.AX25_MINIMUM_I_FRAME_LEN), "length": 0}
    result = ax25.ax25Unstuff(
        packed_ax25_data["data"], packed_ax25_data["length"], unstuffed_packet["data"], unstuffed_packet["length"]
    )
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS
    assert unstuffed_packet["length"] == ax25.AX25_MINIMUM_I_FRAME_LEN

    command = ax25.U_FRAME_CMD_CONN
    result = ax25.ax25Recv(unstuffed_packet, command)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS

    for i in range(fec.RS_ENCODED_SIZE):
        assert telem_data[i] == unstuffed_packet["data"][ax25.AX25_INFO_FIELD_POSITION + i]


# def test_i_frame_max_stuff(telem_data, unstuffed_ax25_data, groundstation_call_sign):
#     """Test sending and receiving an I-frame with maximum bit stuffing."""
#     telem_data[:] = [0xFF] * fec.RS_ENCODED_SIZE
#
#     ax25.setCurrentLinkDestAddress(groundstation_call_sign)
#     result = ax25.ax25SendIFrame(telem_data, len(telem_data), unstuffed_ax25_data)
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     packed_ax25_data = {"data": bytearray(ax25.AX25_MAXIMUM_PKT_LEN), "length": 0}
#     result = ax25.ax25Stuff(unstuffed_ax25_data["data"], unstuffed_ax25_data["length"],
#                             packed_ax25_data["data"], packed_ax25_data["length"])
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#     assert ax25.AX25_MINIMUM_I_FRAME_LEN < packed_ax25_data["length"] < ax25.AX25_MAXIMUM_PKT_LEN
#
#     unstuffed_packet = {"data": bytearray(ax25.AX25_MINIMUM_I_FRAME_LEN), "length": 0}
#     result = ax25.ax25Unstuff(packed_ax25_data["data"], packed_ax25_data["length"],
#                               unstuffed_packet["data"], unstuffed_packet["length"])
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#     assert unstuffed_packet["length"] == ax25.AX25_MINIMUM_I_FRAME_LEN
#
#     command = ax25.ax25Recv(unstuffed_packet)
#     assert command == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     for i in range(fec.RS_ENCODED_SIZE):
#         assert telem_data[i] == unstuffed_packet["data"][ax25.AX25_INFO_FIELD_POSITION + i]
#
# def test_i_frame_some_stuff(telem_data, unstuffed_ax25_data, groundstation_call_sign):
#     """Test sending and receiving an I-frame with some bit stuffing."""
#     seed = 0xF72FA1
#     for i in range(fec.RS_ENCODED_SIZE):
#         seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF
#         telem_data[i] = seed & 0xFF
#
#     ax25.setCurrentLinkDestAddress(groundstation_call_sign)
#     result = ax25.ax25SendIFrame(telem_data, len(telem_data), unstuffed_ax25_data)
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     packed_ax25_data = {"data": bytearray(ax25.AX25_MAXIMUM_PKT_LEN), "length": 0}
#     result = ax25.ax25Stuff(unstuffed_ax25_data["data"], unstuffed_ax25_data["length"],
#                             packed_ax25_data["data"], packed_ax25_data["length"])
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#     assert ax25.AX25_MINIMUM_I_FRAME_LEN < packed_ax25_data["length"] < ax25.AX25_MAXIMUM_PKT_LEN
#
#     unstuffed_packet = {"data": bytearray(ax25.AX25_MINIMUM_I_FRAME_LEN), "length": 0}
#     result = ax25.ax25Unstuff(packed_ax25_data["data"], packed_ax25_data["length"],
#                               unstuffed_packet["data"], unstuffed_packet["length"])
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#     assert unstuffed_packet["length"] == ax25.AX25_MINIMUM_I_FRAME_LEN
#
#     command = ax25.ax25Recv(unstuffed_packet)
#     assert command == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     for i in range(fec.RS_ENCODED_SIZE):
#         assert telem_data[i] == unstuffed_packet["data"][ax25.AX25_INFO_FIELD_POSITION + i]
#
# def test_u_frame_send_recv_conn(packed_ax25_u_frame, cubsat_call_sign):
#     """Test sending and receiving a U-frame with CONN command."""
#     poll_final_bit = 1
#
#     ax25.setCurrentLinkDestAddress(cubsat_call_sign)
#     result = ax25.ax25SendUFrame(packed_ax25_u_frame, ax25.U_FRAME_CMD_CONN, poll_final_bit)
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     unstuffed_packet = {"data": bytearray(ax25.AX25_MINIMUM_I_FRAME_LEN), "length": 0}
#     result = ax25.ax25Unstuff(packed_ax25_u_frame["data"], packed_ax25_u_frame["length"],
#                               unstuffed_packet["data"], unstuffed_packet["length"])
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     command = ax25.ax25Recv(unstuffed_packet)
#     assert command == ax25.U_FRAME_CMD_CONN
#
# def test_u_frame_send_recv_disc(packed_ax25_u_frame, cubsat_call_sign):
#     """Test sending and receiving a U-frame with DISC command."""
#     poll_final_bit = 1
#
#     ax25.setCurrentLinkDestAddress(cubsat_call_sign)
#     result = ax25.ax25SendUFrame(packed_ax25_u_frame, ax25.U_FRAME_CMD_DISC, poll_final_bit)
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     unstuffed_packet = {"data": bytearray(ax25.AX25_MINIMUM_I_FRAME_LEN), "length": 0}
#     result = ax25.ax25Unstuff(packed_ax25_u_frame["data"], packed_ax25_u_frame["length"],
#                               unstuffed_packet["data"], unstuffed_packet["length"])
#     assert result == errors.OBC_GS_ERR_CODE_SUCCESS
#
#     command = ax25.ax25Recv(unstuffed_packet)
#     assert command == ax25.U_FRAME_CMD_DISC
#
# def test_ax25_source_address_generator():
#     """Test generating AX25 source address."""
#     source_address = {"data": bytearray(7), "length": 7}
#     expected_address = bytearray([0x9C, 0x6E, 0x98, 0x8A, 0x9A, 0x40, 0x61])
#     call_sign = b'N7LEM'
#     call_sign2 = b'N7LEM42'
#
#     result = ax25.ax25GetSourceAddress(source_address, call_sign2, 7, 0, 0)
#     assert result == 1
#
#     ax25.ax25GetSourceAddress(source_address, call_sign, 5, 0, 0)
#     assert source_address["data"] == expected_address
#
# def test_ax25_dest_address_generator():
#     """Test generating AX25 destination address."""
#     dest_address = {"data": bytearray(7), "length": 7}
#     expected_address = bytearray([0x9C, 0x94, 0x6E, 0xA0, 0x40, 0x40, 0xE0])
#     call_sign = b'NJ7P'
#     call_sign2 = b'N7LEM42'
#
#     result = ax25.ax25GetDestAddress(dest_address, call_sign2, 7, 0, 0)
#     assert result == 1
#
#     ax25.ax25GetDestAddress(dest_address, call_sign, 4, 0, 1)
#     assert dest_address["data"] == expected_address
