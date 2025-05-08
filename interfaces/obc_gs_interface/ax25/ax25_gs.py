import ax25
import binascii

class AX25:
    """
    A class with the encode and decode methods for the Ax25 Protocol
    """

    GROUND_STATION_CALLSIGN: str = "AKITO"
    CUBE_SAT_CALLSIGN: str = "ATLAS"
    _DEFAULT_SSID: int = 0

    def __init__(self) -> None:
        pass

    def ax25EncodeIFrame(self, data_to_send: str, src_callsign: str = GROUND_STATION_CALLSIGN, dst_callsign: str = CUBE_SAT_CALLSIGN, ns: int = 0) -> bytes:
        """
        Encodes and Information Frame with the requested data using the ax25 library

        :param data_to_send: Data that needs to be sent in the frame
        :param src_callsign: Call sign of the source
        :param dst_callsign: Calls sign of the destination
        :param ns: Send Sequence Number
        :return: True if the frame is successfully made otherwise False
        """
        
        # Generate Frame Object as per Library Specfications
        control_block = ax25.Control(frame_type=ax25.FrameType.I, poll_final=False, send_seqno=ns)
        src_address = ax25.Address(call=src_callsign, ssid=self._DEFAULT_SSID)
        dst_address = ax25.Address(call=dst_callsign, ssid=self._DEFAULT_SSID)
        frame_bytes = bytearray(ax25.Frame(dst=dst_address, src=src_address, via=None, control=control_block, pid=0, data=data_to_send.encode('utf-8')).pack())
        
        # Calculate fcs using CRC 16
        fcs = bytearray(binascii.crc_hqx(frame_bytes, 0).to_bytes(2, 'big'))
        # Define the flags
        flag = bytearray(bytes.fromhex("7E"))
        # Use the mutability of bytearrays to append everything into a huge bytearray that contains what we want to send
        frame_bytes = flag + frame_bytes + fcs + flag
        # Convert the bytearray to bytes
        return_frame = bytes(frame_bytes)

        return return_frame;
    
    def ax25DecodeIFrame(self, data: bytes) -> ax25.Frame:
        """
        Encodes and Information Frame with the requested data using the ax25 library

        :param data_to_send: Data that needs to be sent in the frame
        :param src_callsign: Call sign of the source
        :param dst_callsign: Calls sign of the destination
        :param ns: Send Sequence Number
        :return: True if the frame is successfully made otherwise False
        """
        # Extract the original 2 fcs bytes (16 bits)
        fcs_original = int.from_bytes(data[-3:-1], byteorder='big', signed=True)

        # Use splicing to get rid of the start and end byte and 2 fcs bytes
        data  = data[1:-3]

        # Calculate fcs of recieved frame
        fcs_data = binascii.crc_hqx(data, 0)

        # TODO: Error handle this properly
        if(fcs_original != fcs_data):
            print("Oh no")

        return ax25.Frame.unpack(data)   


# Example Usage
coder = AX25()
frame = coder.ax25EncodeIFrame("A", AX25.CUBE_SAT_CALLSIGN, AX25.GROUND_STATION_CALLSIGN, 0)
print(frame)
print(coder.ax25DecodeIFrame(frame).dst)
