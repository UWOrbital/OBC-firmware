import time
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
        print("Receiving from bootloader...")
        print(ser.read(len("Waiting for input\r\n")))
        start = time.time()
        print(ser.read(len("Jumping to app location\r\n")))
        end = time.time()
        print(end - start)
