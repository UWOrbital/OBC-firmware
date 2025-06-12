from time import sleep
from typing import Final

from serial import PARITY_NONE, STOPBITS_TWO, Serial

# TEST SCRIPT TO WRITE TO BL

OBC_UART_BAUD_RATE: Final[int] = 115200

if __name__ == "__main__":
    with Serial(
        "/dev/ttyACM0",
        baudrate=OBC_UART_BAUD_RATE,
        parity=PARITY_NONE,
        stopbits=STOPBITS_TWO,
        timeout=4,
    ) as ser:
        # Ping Test 0x05
        # print("Writing to bootloader...")
        # write_bytes = b"\x05".ljust(223, b"\x00")
        # print(write_bytes)
        # ser.write(write_bytes)
        # print("Receiving from bootloader...")
        # print(ser.read(20))
        # sleep(1)
        # print("Writing to bootloader...")
        # write_bytes = b"\x05".ljust(223, b"\x00")
        # print(write_bytes)
        # ser.write(write_bytes)
        # print("Receiving from bootloader...")
        # print(ser.read(20))
        # Set Programming Session Test 0x07
        print("Writing to bootloader...")
        write_bytes = b"\x07".ljust(223, b"\x00")
        print(write_bytes)
        ser.write(write_bytes)
        print("Receiving from bootloader...")
        print(ser.read(5))
        sleep(1)
        print("Writing to bootloader...")
        write_bytes = b"\x07\x00\x00\x00\x00\x01".ljust(223, b"\x00")
        print(write_bytes)
        ser.write(write_bytes)
        print("Receiving from bootloader...")
        print(ser.read(5))
