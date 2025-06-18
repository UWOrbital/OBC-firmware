from typing import Final

from serial import PARITY_NONE, STOPBITS_TWO, Serial

# TEST SCRIPT TO WRITE TO BL

OBC_UART_BAUD_RATE: Final[int] = 115200


def send_data(ser: Serial, write_bytes: bytes) -> None:
    """
    Testing send data function
    """
    print("Writing to bootloader...")
    print(write_bytes)
    ser.write(write_bytes)
    print("Receiving from bootloader...")
    print(ser.read(100))


if __name__ == "__main__":
    with Serial(
        "/dev/ttyACM0",
        baudrate=OBC_UART_BAUD_RATE,
        parity=PARITY_NONE,
        stopbits=STOPBITS_TWO,
        timeout=7,
    ) as ser:
        # Ping Test 0x05
        # send_data(ser, b"\x05".ljust(223, b"\x00"))
        # sleep(1)
        # send_data(ser, b"\x05".ljust(223, b"\x00"))

        # Set prorammning session test 0x07
        # send_data(ser, b"\x07\x00\x00\x00\x00\x01".ljust(223, b"\x00"))

        # Download data test
        send_data(ser, b"\x09\x00\x00\x00\x00\x01\x00\xd0\x00\x04\x00\x00".ljust(223, b"\xff"))
