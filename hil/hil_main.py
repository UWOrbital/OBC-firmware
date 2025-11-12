import serial

from obc.tools.python.app_update import main as flash_main


def main() -> None:
    """Hardware-in-the-loop (HIL) test script for OBC communication."""
    # bootloader flashing
    flash_main()

    # setup the serial port with baudrate (baudrate in interfaces/init.py)
    ser = serial.Serial(port="dev/ttyACM0", baudrate=115200)

    # test writing and reading
    ser.write(b"Hello OBC\n")
    response = ser.readline()

    # convert to string from binary
    strresponse = str(response, "UTF-8")
    print(f"Response {strresponse}")

    # close the port
    ser.close()


if __name__ == "__main__":
    main()
