import ax25
import sys

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

        control_block = ax25.Control(frame_type=ax25.FrameType.I, poll_final=False, send_seqno=ns)
        src_address = ax25.Address(call=src_callsign, ssid=self._DEFAULT_SSID)
        dst_address = ax25.Address(call=dst_callsign, ssid=self._DEFAULT_SSID)
        frame_bytes = ax25.Frame(dst=dst_address, src=src_address, via=None, control=control_block, pid=0, data=data_to_send.encode('utf-8')).pack()
        
        return frame_bytes;
    
    def ax25DecodeIFrame(self, data: bytes) -> ax25.Frame:
        """
        Encodes and Information Frame with the requested data using the ax25 library

        :param data_to_send: Data that needs to be sent in the frame
        :param src_callsign: Call sign of the source
        :param dst_callsign: Calls sign of the destination
        :param ns: Send Sequence Number
        :return: True if the frame is successfully made otherwise False
        """
        
        return ax25.Frame.unpack(data)



coder = AX25()
frame = coder.ax25EncodeIFrame("Hallo", AX25.CUBE_SAT_CALLSIGN, AX25.GROUND_STATION_CALLSIGN, 0)
print(frame)
print(coder.ax25DecodeIFrame(frame).dst)
