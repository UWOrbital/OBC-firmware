from binascii import crc_hqx

from ax25 import Address, Control, Frame, FrameType
from pyStuffing import BitStuffing

from interfaces import AX25_NON_INFO_BYTES, RS_ENCODED_DATA_SIZE


class AX25:
    """
    A class with the encode and decode methods for the Ax25 Protocol
    """

    _DEFAULT_SSID: int = 0
    _CRC_LENGTH_BITS = 16

    def __init__(self, src: str, dst: str) -> None:
        """
        :param src: Source Address Callsign (Should be a maximum of 6 characters long)
        :param dst: Destination Address Callsign (Should be a maximum of 6 characters long)
        """
        if Address.valid_call(src) and Address.valid_call(dst):
            self.src_callsign = src
            self.dst_callsign = dst
        elif not Address.valid_call(src) and not Address.valid_call(dst):
            raise ValueError("Both Addresses are not valid")
        elif not Address.valid_call(src):
            raise ValueError("Source Address is not valid")
        elif not Address.valid_call(dst):
            raise ValueError("Destination Address is not valid")

    def encode_frame(
        self, data_to_send: bytes | None, frame_type: FrameType, sequence_number: int = 0, poll: bool = False
    ) -> bytes:
        """
        Encodes and Information Frame with the requested data using the ax25 library
        Note: The source and destination call signs passed in the constructor of the class are used

        :param data_to_send: Data that needs to be sent in the frame
        :param frame_type: The type of frame being created (this directly influences the control field bytes)
        :param sequence_number: Send Sequence Number, by default this is 0
        :param poll: Whether the poll bit is used or not (this is for the U Frames that are used to esthablish a
                     connection with the obc). By default, this is set to false
        :return: Generated Frame
        """

        # Generate Frame Object as per Library Specfications
        control_block = Control(frame_type, poll_final=poll, send_seqno=sequence_number)
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
        initial_crc_value = 0
        binary = bin(crc_hqx(frame_bytes, initial_crc_value))
        reverse = binary[-1:1:-1]
        reverse = reverse + (self._CRC_LENGTH_BITS - len(reverse)) * "0"
        fcs = bytearray(int(reverse, self._CRC_LENGTH_BITS // 8).to_bytes(self._CRC_LENGTH_BITS // 8, "big"))

        frame_bytes = frame_bytes + fcs

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

        :param input_data: Unstuffed frame in bytes
        :return: The decoded frame
        """
        data = input_data[1:-1]

        # Get the FCS flags from the original data transmission
        fcs_original = int.from_bytes(data[-2:], byteorder="big", signed=False)
        # Remove the fcs flags
        data = data[:-2]

        # Calculate fcs of recieved frame and then reversing it
        binary = bin(crc_hqx(data, 0))
        reverse = binary[-1:1:-1]
        reverse = reverse + (self._CRC_LENGTH_BITS - len(reverse)) * "0"
        fcs_data = int(reverse, self._CRC_LENGTH_BITS // 8)

        if fcs_original != fcs_data:
            raise ValueError("Check sums do not match")

        return Frame.unpack(data)

    def unstuff(self, input_data: bytes) -> bytes:
        """
        Unstuffs frames passed in as bytes using the pyStuffing library.

        :param input_data: Stuffed frame to be unstuffed as bytes
        :return: The unStuffed frame
        """
        data = input_data[1:-1]

        int_list = self._bytes_to_int_list(data)

        unstuff = BitStuffing(int_list)
        unstuff.stuffed = int_list
        unstuff.startUnstuffing()
        data = self._int_list_to_bytes(unstuff.unStuffed)

        # Remove a 0 at the end of the string that might have been created as a result of adding in 0s
        # There is a small chance that the last fcs byte is 0 so we check if the data size is bigger than it's supposed
        # to be
        # We also check if the frame is a U frame in which case it has to be less than RS_ENCODED_DATA_SIZE
        # Only remove the byte if we're certain it's padding (exactly 1 byte more than expected for I frames)
        if data[-1] == 0 and len(data) == RS_ENCODED_DATA_SIZE + AX25_NON_INFO_BYTES + 1:
            # I frame with exactly 1 byte of padding from bit-to-byte conversion
            data = data[:-1]
        elif data[-1] == 0 and len(data) < RS_ENCODED_DATA_SIZE:
            # U frame - maintain existing behavior for backward compatibility
            data = data[:-1]

        data_bytes = bytearray(data)
        start_end_flag = bytearray(bytes.fromhex("7E"))

        return bytes(start_end_flag + data_bytes + start_end_flag)

    def stuff(self, input_data: bytes) -> bytes:
        """
        Stuffs frames passed in as bytes using the pyStuffing library.

        :param input_data: Unstuffed frame to be stuffed as bytes
        :return: The stuffed frame
        """
        data_stripped = input_data[1:-1]

        unstuff = BitStuffing(self._bytes_to_int_list(data_stripped))
        unstuff.startStuffing()
        data_bytes = bytearray(self._int_list_to_bytes(unstuff.stuffed)).rstrip(b"\x00")

        # Define the flags
        start_end_flag = bytearray(bytes.fromhex("7E"))

        return bytes(start_end_flag + data_bytes + start_end_flag)

    def _bytes_to_int_list(self, data: bytes) -> list[int]:
        """
        A Function that converts bytes to a list of integers representing bits

        :param data: The data that needs to be converted
        :return: A list of integers representing bits of the data parameter
        """
        byte_list = []
        for byte in data:
            bits = bin(byte).removeprefix("0b")
            byte_list.append(("0" * (8 - len(bits))) + bits)
        byte_string = "".join(byte_list)
        int_list = [int(s) for s in byte_string]
        return int_list

    def _int_list_to_bytes(self, data: list[int]) -> bytes:
        """
        A Function that converts a list of integers to bytes

        :param data: A list of integers representing bits that need to be converted to bytes
        :return: The bytes that the data parameter was converted to
        """
        # Join all the integers into a string
        res = "".join([str(s) for s in data])
        # Make sure that this string is a multiple of 8 by adding 0s at the end (this makes sure that the last byte was
        # not cut off)
        res = res + ("0" * (8 - len(res) % 8))
        # Loop through every 8 characters and convert the bits to an integer which we can then convert to a byte
        data_bytes = bytes(int(res[i : i + 8], 2) for i in range(0, len(res), 8))
        return data_bytes


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
