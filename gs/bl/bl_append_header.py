from dataclasses import dataclass
import struct
import serial
import argparse
import sys
from pathlib import Path


# Define header class. Example dummy data
@dataclass
class header_struct:
    version: int
    bin_size: int
    data: int

    def serialize(self, big_endian: bool) -> bytes:
        if (big_endian):
            return struct.pack('>III', self.version, self.bin_size, self.data)
        else:
            return struct.pack('<III', self.version, self.bin_size, self.data)


def create_bin(input_path: str, input_version: int, header_data: int) -> str:
    """
    Appends custom header data to a .bin file

    :param input_path: Path to .bin file to be modified and sent
    :param input_version: Header version (dummy value)
    :param header_data: Custom header data (dummy value)
    :param com_port: Com port for UART communication
    """

    input_obj = Path(input_path)

    input_bin_data = input_obj.read_bytes()
    bin_len = len(input_bin_data)
    header = header_struct(version=input_version, bin_size=bin_len, data=header_data) # Create header struct with data
    header_bytes = header.serialize(True)

    output_path = input_path.replace('.bin', '_formatted.bin')
    output_obj = Path(output_path)
    output_obj.write_bytes(header_bytes + input_bin_data)

    return output_path



def send_bin(file_path:str, com_port: str) -> None:
    """
    Sends .bin file over UART serial port

    :param file_path: Path to .bin file to be sent
    :param com_port: Com port for UART communication
    """

    OBC_UART_BAUD_RATE = 115200
    file_obj = Path(file_path)

    with serial.Serial(com_port, baudrate=OBC_UART_BAUD_RATE, parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE, timeout=1) as ser: # Open serial port
       data = file_obj.read_bytes()
       ser.write(data) # Write binary to device via UART

def main():

    args = parser.parse_args() # Parse args
    output_file = create_bin(args.input_path, args.version, args.data)
    send_bin(output_file, args.port)

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Append custom data to .bin and send')

    if len(sys.argv) == 1:
        parser.error("Error: Please provide required arguments")

    # Add arguments
    parser.add_argument('-i', required=True, dest='input_path', type=str, help='Path to the input .bin file.')
    parser.add_argument('-p', required=True, dest='port', type=str, help='Serial port number')
    parser.add_argument('-d', required=True, dest='data', type=int, help='Custom data to be sent')
    parser.add_argument('-v', dest='version', type=int, default=0, help='Version of the application. Default is 0')

    main()
