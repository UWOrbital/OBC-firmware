#!/usr/bin/env python3
import sys
import time

from serial import Serial

APP_BAUD_RATE = 115200


def uart_test(port: str) -> None:
    """Write and receive a simple UART test."""
    with Serial(port, baudrate=APP_BAUD_RATE, timeout=2) as ser:
        print("Waiting for app connection...")
        time.sleep(1)

        ser.reset_input_buffer()
        ser.reset_output_buffer()

        ser.write(b"THIS IS A TEST STRING - HELLO!\n")
        response = ser.readline().decode(errors="replace").strip()

        if response:
            print("App responded:", response)
        else:
            print("No response :(")


def main() -> None:
    """Check for args; need to specify COM port."""
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <COM_PORT>")
        return

    uart_test(sys.argv[1])


if __name__ == "__main__":
    main()
