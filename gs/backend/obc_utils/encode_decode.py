from ax25 import Frame, FrameType

from interfaces import (
    CUBE_SAT_CALLSIGN,
    GROUND_STATION_CALLSIGN,
    INFO_FIELD_END_POSITION,
    INFO_FIELD_START_POSITION,
    RS_DECODED_DATA_SIZE,
)
from interfaces.obc_gs_interface.aes128 import AES128
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.fec import FEC


def encode(data: bytes) -> bytes:
    """
    Given data that is less than 223 bytes, this function applies fec and aes128 as well as frames the data in
    accordance with the ax25 protocol with bit stuffing

    :param data: The data to be encoded
    :return: The function returns the encoded bytes. Note, these bytes are stuffed
    """
    if len(data) > RS_DECODED_DATA_SIZE:
        raise ValueError("Invalid Argument: Data to encode is too long, it must be shorter or equal to 223 bytes")

    # Instantiate our ax25 class to get ready to create frame
    ax25 = AX25(GROUND_STATION_CALLSIGN, CUBE_SAT_CALLSIGN)
    # Instantiate the fec class for forward error correction
    fec = FEC()
    # Instantiate the aes cipher with the same defaults from the c implementation
    aes = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    # TODO: AES is hard-coded, this will need to be changed. Additionally the iv will need to be framed

    # Encrypt data
    encrypted_data = aes.encrypt(data)
    # Encode data for error correction
    encode_data = fec.encode(bytes(encrypted_data))
    # Create the frame
    send_frame = ax25.encode_frame(encode_data, FrameType.I, 0)
    # Stuff the frame as per the standard
    send_frame_stuffed = ax25.stuff(send_frame)

    return send_frame_stuffed


def decode(data: bytes) -> Frame:
    """
    Given a stuffed ax25 frame with fec and aes128, this function decodes the data into a usable Frame object from the
    pyham_ax25 library

    :param data: Bytes containing data with fec, aes128 and ax25 framing with bit stuffing
    :return: A Frame object representing that data
    """
    # Instantiate our ax25 class to get ready to decode frame
    ax25 = AX25(GROUND_STATION_CALLSIGN, CUBE_SAT_CALLSIGN)
    # Instantiate the fec class for forward error correction
    fec = FEC()
    # Instantiate the aes cipher with the same defaults from the c implementation
    aes = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    # TODO: AES is hard-coded, this will need to be changed. Additionally the iv will need to be framed

    # Unstuff the data
    data_unstuffed = ax25.unstuff(bytes(data))
    # NOTE: 17 (inclusive) to 272 (exclusive) is the range for info bytes that are needed for the decoding
    fec_data = fec.decode(data_unstuffed[INFO_FIELD_START_POSITION : INFO_FIELD_END_POSITION + 1])
    # With the data decoded we need to add the rest of the data back to get a full frame
    decoded_data = bytes(
        data_unstuffed[:INFO_FIELD_START_POSITION] + fec_data + data_unstuffed[INFO_FIELD_END_POSITION + 1 :]
    )
    # Turn the bytes into the frame using the ax25 library
    # We don't do aes decryption as the fcs values won't match, thus, we create and return a new frame instead
    rcv_frame = ax25.decode_frame(decoded_data)

    # Decrypt the data is there is any
    decrypted_data = None if rcv_frame.data is None else aes.decrypt(bytes(rcv_frame.data))

    # Create a new frame with decrypted data and return it
    rcv_frame = Frame(
        dst=rcv_frame.dst,
        src=rcv_frame.src,
        via=None,
        control=rcv_frame.control,
        pid=rcv_frame.pid,
        data=decrypted_data,
    )

    return rcv_frame
