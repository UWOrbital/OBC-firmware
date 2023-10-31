from dataclasses import dataclass
import struct
import serial
import argparse

# Define header struct. Example dummy struct
@dataclass
class header_struct:
    version: int
    bin_size: int
    data: int
    
def send_bin(input_file, input_version, header_data, com_port):
        
        with open(input_file, 'rb') as f:
            bin_data = f.read()
            
        bin_len = len(bin_data)
        header = header_struct(version=input_version, bin_size=bin_len, data=header_data) # Create header struct with data
        
        header_bytes = struct.pack('III', header.version, header.bin_size, header.data)
        
        output_file = input_file.replace('.bin', '_header.bin')
        
        with open(output_file, 'wb') as f: # Create output binary
            f.write(header_bytes)
            f.write(bin_data)

        
        OBC_UART_BAUD_RATE = 115200
        
        with serial.Serial(com_port, OBC_UART_BAUD_RATE, timeout=1) as ser: # Open serial port
            with open(input_file, 'rb') as f:
                data = f.read()
                ser.write(data) # Write binary to device via UART
            ser.close()

        
def main():
    parser = argparse.ArgumentParser(description='Append custom data to .bin and send')
    
    # Add arguments
    parser.add_argument('-i', '--input_file', required=True, help='Path to the input .bin file.')
    parser.add_argument('-p', '--port', required=True, type=str, help='Serial port')
    parser.add_argument('-d', '--data', required=True, type=int, help='Custom data to be sent')
    parser.add_argument('-v', '--version', type=int, default=1, help='Version of the header. Default is 1.')

    # Parse args
    args = parser.parse_args()
    send_bin(args.input_file, args.version, args.data, args.port)
    
if __name__ == "__main__":
    main()

    