from binascii import crc_hqx

from ax25 import Address, Control, Frame, FrameType


class AX25:
    """
    A class with the encode and decode methods for the Ax25 Protocol
    """

    _DEFAULT_SSID: int = 0

    def __init__(self, src: str, dst: str) -> None:
        if Address.valid_call(src) and Address.valid_call(dst):
            self.src_callsign = src
            self.dst_callsign = dst
        else:
            raise Exception("Invalid call sign for source or destination")

    def encode_frame(
        self,
        data_to_send: bytes,
        frame_type: FrameType,
        ns: int = 0,
    ) -> bytes:
        """
        Encodes and Information Frame with the requested data using the ax25 library
        Note: The source and destination call signs passed in the constructor of the class are used

        :param data_to_send: Data that needs to be sent in the frame
        :param ns: Send Sequence Number
        :return: Generated Frame
        """

        # Generate Frame Object as per Library Specfications
        control_block = Control(frame_type, poll_final=False, send_seqno=ns)
        src_address = Address(call=self.src_callsign, ssid=self._DEFAULT_SSID)
        dst_address = Address(call=self.dst_callsign, ssid=self._DEFAULT_SSID)
        frame_bytes = bytearray(
            Frame(
                dst=dst_address,
                src=src_address,
                via=None,
                control=control_block,
                pid=0,
                data=data_to_send,
            ).pack()
        )

        # Calculate fcs using CRC 16
        fcs = bytearray(crc_hqx(frame_bytes, 0).to_bytes(2, "big"))

        # Define the flags
        start_end_flag = bytearray(bytes.fromhex("7E"))

        # Use the mutability of bytearrays to append everything into a huge bytearray that contains what we want to send
        frame_bytes = start_end_flag + frame_bytes + fcs + start_end_flag

        # Convert the bytearray to bytes
        return_frame = bytes(frame_bytes)

        return return_frame

    def decode_frame(self, data: bytes) -> Frame:
        """
        Decodes frames passed in as bytes using the ax25 library.

        :return: The decoded frame
        """
        # Extract the original 2 fcs bytes (16 bits)
        fcs_original = int.from_bytes(data[-3:-1], byteorder="big", signed=False)

        # Use splicing to get rid of the start and end byte and 2 fcs bytes
        data = data[1:-3]

        # Calculate fcs of recieved frame
        fcs_data = crc_hqx(data, 0)

        if fcs_original != fcs_data:
            raise Exception("The data has been corrupted (FCS sequences do not match)")

        return Frame.unpack(data)


# Example Usage
if __name__ == "__main__":
    comm_1 = AX25("ATLAS", "AKITO")
    send_frame = comm_1.encode_frame(b"UW Orbital", FrameType.I, 0)
    print(send_frame)

    rcv_frame = comm_1.decode_frame(send_frame)
    print("Source: " + str(rcv_frame.src))
    print("Destination: " + str(rcv_frame.dst))
    print("Frame Type: " + str(rcv_frame.control.frame_type))
    print("Data: " + str(rcv_frame.data.decode("UTF-8")))
    print("")

    comm_2 = AX25("LEAFS", "PLUTO")
    send_frame = comm_2.encode_frame(b"Leafs in four", FrameType.U, 0)
    print(send_frame)

    rcv_frame = comm_2.decode_frame(send_frame)
    print("Source: " + str(rcv_frame.src))
    print("Destination: " + str(rcv_frame.dst))
    print("Frame Type: " + str(rcv_frame.control.frame_type))
    print("Data: " + str(rcv_frame.data.decode("UTF-8")))
