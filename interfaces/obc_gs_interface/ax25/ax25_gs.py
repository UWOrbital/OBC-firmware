from binascii import crc_hqx

from ax25 import Address, Control, Frame, FrameType


class AX25:
    """
    A class with the encode and decode methods for the Ax25 Protocol
    """

    GROUND_STATION_CALLSIGN: str = "AKITO"
    CUBE_SAT_CALLSIGN: str = "ATLAS"
    _DEFAULT_SSID: int = 0

    def __init__(self) -> None:
        pass

    def encode_frame(
        self,
        data_to_send: bytes,
        frame_type: FrameType,
        src_callsign: str = GROUND_STATION_CALLSIGN,
        dst_callsign: str = CUBE_SAT_CALLSIGN,
        ns: int = 0,
    ) -> bytes:
        """
        Encodes and Information Frame with the requested data using the ax25 library

        :param data_to_send: Data that needs to be sent in the frame
        :param src_callsign: Call sign of the source
        :param dst_callsign: Calls sign of the destination
        :param ns: Send Sequence Number
        :return: Generated Frame
        """

        # Generate Frame Object as per Library Specfications
        control_block = Control(frame_type, poll_final=False, send_seqno=ns)
        src_address = Address(call=src_callsign, ssid=self._DEFAULT_SSID)
        dst_address = Address(call=dst_callsign, ssid=self._DEFAULT_SSID)
        frame_bytes = bytearray(
            Frame(
                dst=dst_address,
                src=src_address,
                via=None,
                control=control_block,
                pid=0,
                data=data_to_send.encode("utf-8"),
            ).pack()
        )

        # Calculate fcs using CRC 16
        fcs = bytearray(crc_hqx(frame_bytes, 0).to_bytes(2, "big"))
        # Define the flags
        flag = bytearray(bytes.fromhex("7E"))
        # Use the mutability of bytearrays to append everything into a huge bytearray that contains what we want to send
        frame_bytes = flag + frame_bytes + fcs + flag
        # Convert the bytearray to bytes
        return_frame = bytes(frame_bytes)

        return return_frame

    def decode_frame(self, data: bytes) -> Frame:
        """
        Encodes and Information Frame with the requested data using the ax25 library

        :param data_to_send: Data that needs to be sent in the frame
        :param src_callsign: Call sign of the source
        :param dst_callsign: Calls sign of the destination
        :param ns: Send Sequence Number
        :return: True if the frame is successfully made otherwise False
        """
        # Extract the original 2 fcs bytes (16 bits)
        fcs_original = int.from_bytes(data[-3:-1], byteorder="big", signed=True)

        # Use splicing to get rid of the start and end byte and 2 fcs bytes
        data = data[1:-3]

        # Calculate fcs of recieved frame
        fcs_data = crc_hqx(data, 0)

        if fcs_original != fcs_data:
            raise Exception("The data has been corrupted (FCS sequences do not match)")

        return Frame.unpack(data)


# Example Usage
if __name__ == "__main__":
    coder = AX25()
    frame = coder.encode_frame("A", FrameType.I, AX25.CUBE_SAT_CALLSIGN, AX25.GROUND_STATION_CALLSIGN, 0)
    print(frame)
    print(coder.decode_frame(frame).dst)
