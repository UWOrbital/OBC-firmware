#!/usr/bin/env python3
import argparse
import sys
import time

import serial

from obc.tools.python.app_update import send_bin  # call this directly

APP_BAUD = 115200  # app UART baud (adjust if your app uses a different rate)
APP_TIMEOUT_S = 2.0  # read timeout for tests
POST_FLASH_DELAY_S = 2.0  # time to let BL jump to app before opening serial


def hil_test(port: str) -> None:
    """Minimal smoke test: write a line, read a line."""
    with serial.Serial(
        port=port,
        baudrate=APP_BAUD,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,  # app is usually 8N1
        timeout=APP_TIMEOUT_S,
    ) as ser:
        # give the app a moment, then clear any boot text
        time.sleep(0.3)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        ser.write(b"Test OBC\n")
        line = ser.readline()
        print(f"Response {line.decode('utf-8', errors='replace').rstrip()}")


def main() -> None:
    "Main String DocString"
    p = argparse.ArgumentParser(description="Flash OBC via bootloader, then run HIL smoke test.")
    p.add_argument("port", help="Serial port")
    p.add_argument("bin_path", help="Path to OBC-firmware-crc.bin")
    args = p.parse_args()

    # 1) Flash the app via bootloader (bootloader uses 115200, 8N2 inside send_bin)
    print(f"[HIL] Flashing {args.bin_path} over {args.port}")
    send_bin(args.bin_path, args.port)

    # 2) Let bootloader jump to application
    print(f"[HIL] Waiting {POST_FLASH_DELAY_S:.1f}s for app to start")
    time.sleep(POST_FLASH_DELAY_S)

    # 3) Run a simple UART test against the app
    print("[HIL] Running smoke test")
    try:
        hil_test(args.port)
        print("[HIL] Smoke test complete")
    except serial.SerialException as e:
        print(f"[HIL] Serial error: {e}", file=sys.stderr)
        sys.exit(2)


if __name__ == "__main__":
    main()
