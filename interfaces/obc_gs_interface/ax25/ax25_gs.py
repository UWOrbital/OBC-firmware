from binascii import crc_hqx

from ax25 import Address, Control, Frame, FrameType
from pyStuffing import BitStuffing


class AX25:
    """
    A class with the encode and decode methods for the Ax25 Protocol
    """

    _DEFAULT_SSID: int = 0

    def __init__(self, src: str, dst: str) -> None:
        if Address.valid_call(src) and Address.valid_call(dst):
            self.src_callsign = src
            self.dst_callsign = dst

    def encode_frame(
        self,
        data_to_send: bytes,
        frame_type: FrameType,
        sequence_number: int = 0,
    ) -> bytes:
        """
        Encodes and Information Frame with the requested data using the ax25 library
        Note: The source and destination call signs passed in the constructor of the class are used

        :param data_to_send: Data that needs to be sent in the frame
        :param ns: Send Sequence Number
        :return: Generated Frame
        """

        # Generate Frame Object as per Library Specfications
        control_block = Control(frame_type, poll_final=False, send_seqno=sequence_number)
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

        # Calculate fcs using CRC 16 and then reverse it
        binary = bin(crc_hqx(frame_bytes, 0))
        reverse = binary[-1:1:-1]
        reverse = reverse + (16 - len(reverse)) * "0"
        fcs = bytearray(int(reverse, 2).to_bytes(2, "big"))
        print(fcs)

        frame_bytes = frame_bytes + fcs

        byte_list = []
        for byte in frame_bytes:
            bits = bin(byte).removeprefix("0b")
            byte_list.append(("0" * (8 - len(bits))) + bits)
        byte_string = "".join(byte_list)
        bin_list = [int(s) for s in byte_string]

        unstuff = BitStuffing(bin_list)
        unstuff.startStuffing()
        res = "".join([str(s) for s in unstuff.stuffed])
        res = res + ("0" * (8 - len(res) % 8))
        frame_bytes = bytearray(bytes(int(res[i : i + 8], 2) for i in range(0, len(res), 8)))

        print(frame_bytes)
        # Define the flags
        start_end_flag = bytearray(bytes.fromhex("7E"))

        # Use the mutability of bytearrays to append everything into a huge bytearray that contains what we want to send
        frame_bytes = start_end_flag + frame_bytes + start_end_flag

        # Convert the bytearray to bytes
        return_frame = bytes(frame_bytes)

        return return_frame

    def decode_frame(self, input_data: bytes) -> Frame:
        """
        Decodes frames passed in as bytes using the ax25 library.

        :return: The decoded frame
        """
        data = input_data[1:-1]

        byte_list = []
        for byte in data:
            bits = bin(byte).removeprefix("0b")
            byte_list.append(("0" * (8 - len(bits))) + bits)
        byte_string = "".join(byte_list)
        bin_list = [int(s) for s in byte_string]
        print(data)
        print(byte_list)

        unstuff = BitStuffing(bin_list)
        unstuff.stuffed = bin_list
        unstuff.startUnstuffing()
        res = "".join([str(s) for s in unstuff.unStuffed])
        res = res + ("0" * (8 - len(res) % 8))
        data = bytes(int(res[i : i + 8], 2) for i in range(0, len(res), 8))

        # Remove a 0 at the end of the string that might have been created as a result of adding in 0s
        if data[-1] == 0:
            data = data[:-1]

        # Get the FCS flags from the original data transmission
        fcs_original = int.from_bytes(data[-2:], byteorder="big", signed=False)
        # Remove the fcs flags
        data = data[:-2]

        # Calculate fcs of recieved frame and then reversing it
        binary = bin(crc_hqx(data, 0))
        reverse = binary[-1:1:-1]
        reverse = reverse + (16 - len(reverse)) * "0"
        fcs_data = int(reverse, 2)
        print(fcs_original)
        print(fcs_data)

        if fcs_original != fcs_data:
            raise ValueError(data)

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

    comm_2 = AX25("LEAFS", "CANUCK")
    send_frame = comm_2.encode_frame(b"Leafs in four", FrameType.U, 0)
    print(send_frame)

    rcv_frame = comm_2.decode_frame(send_frame)
    print("Source: " + str(rcv_frame.src))
    print("Destination: " + str(rcv_frame.dst))
    print("Frame Type: " + str(rcv_frame.control.frame_type))
    print("Data: " + str(rcv_frame.data.decode("UTF-8")))
