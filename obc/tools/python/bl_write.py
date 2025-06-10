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
        timeout=5,
    ) as ser:
        print("Writing to bootloader...")
        # 0x05 Represents the CMD_PING id
        write_bytes = b"\x05".ljust(16, b"\x00")
        print(write_bytes)
        ser.write(write_bytes)
        print("Receiving from bootloader...")
        print(ser.read(1))
