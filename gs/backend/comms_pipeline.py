import time
from argparse import ArgumentParser

import serial
from ax25 import FrameType

from interfaces import OBC_UART_BAUD_RATE, RS_DECODED_DATA_SIZE
from interfaces.command_framing import command_frame
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdMsg,
    create_cmd_uplink_disc,
)
from interfaces.obc_gs_interface.fec import FEC


def send_command(command: CmdMsg, com_port: str) -> None:
    """
    A function to send a command up to the cube satellite
    """
    data = [command]
    send_bytes = command_frame(data).ljust(300, b"\x00")

    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = FEC()
    print("-----Send Frame Bytes-----")
    print([hex(byte) for byte in send_bytes])
    print("Length: " + str(len(send_bytes)))

    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        ser.reset_output_buffer()
        ser.write(send_bytes)
        print("Frame Sent")

        rcv_frame_bytes = ser.read(10000)
        start_index = rcv_frame_bytes.find(b"\x7e")
        end_index = rcv_frame_bytes.rfind(b"\x7e")
        print(str(start_index) + " " + str(end_index))

        if start_index != 0:
            print(rcv_frame_bytes[:start_index].decode("utf-8"))
            print(rcv_frame_bytes[end_index:].decode("utf-8"))

        read_bytes = rcv_frame_bytes[start_index : end_index + 1]

    frame_data_bytes = b""
    print("---- Stuffed Frame -----")
    print([hex(byte) for byte in read_bytes])
    print("Length: " + str(len(read_bytes)))
    stuffed_frame = bytes(read_bytes)

    if len(read_bytes) != 0:
        frame_data_bytes = ax25_proto.unstuff(stuffed_frame)

    print("---- Unstuffed Frame -----")
    print([hex(byte) for byte in frame_data_bytes])
    print("Length: " + str(len(frame_data_bytes)))

    if len(read_bytes) != 0:
        # NOTE: 17 (inclusive) to 272 (exclusive) is the range for info bytes that are needed for the decoding
        if len(read_bytes) > 80:
            data_to_decode = fec_coder.decode(frame_data_bytes[17:272])
            # With the data decoded we need to add the rest of the data back to get a full frame
            decoded_data = bytes(frame_data_bytes[:17] + data_to_decode + frame_data_bytes[272:])
            # Now we can finally decode the frame and extract information
            rcv_frame = ax25_proto.decode_frame(decoded_data)
            frame_data = rcv_frame.data
            print("---- Frame Data (First 223 Bytes) -----")
            print([hex(byte) for byte in frame_data[:RS_DECODED_DATA_SIZE]])
        else:
            rcv_frame = ax25_proto.decode_frame(frame_data_bytes)
            print("---- Frame Data-----")
            print("Frame type: " + str(rcv_frame.control.frame_type))


def arg_parse() -> ArgumentParser:
    """
    Argument Parsing for the send and receive commands
    """
    # TODO: Add receive
    parser = ArgumentParser(
        prog="UW Orbital CLI",
        description="Sending and Receiving from the satellite using the command line",
        epilog="Help needs implementation",
    )
    parser.add_argument(
        "-c", "--command", required=True, dest="command", type=str, choices=[x.name for x in CmdCallbackId]
    )
    parser.add_argument("-t", "--timestamp", required=False, dest="timestamp", type=int)
    parser.add_argument("-rtc", "--rtc_sync_time", required=False, dest="rtc_sync", type=int)
    parser.add_argument("-lnp", "--log_next_pass", required=False, dest="log_level", type=int)

    return parser


if __name__ == "__main__":
    with serial.Serial(
        "/dev/ttyUSB0",
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        ax25_proto = AX25("ATLAS", "AKITO")
        send_bytes = ax25_proto.encode_frame(None, FrameType.SABM, 0, True)
        send_bytes = ax25_proto.stuff(send_bytes)
        print([hex(byte) for byte in send_bytes])
        ser.write(send_bytes.ljust(30, b"\x00"))
        print("Frame Sent")
        rcv_frame_bytes = ser.read(10000)
        start_index = rcv_frame_bytes.find(b"\x7e")
        end_index = rcv_frame_bytes.rfind(b"\x7e")
        print(str(start_index) + " " + str(end_index))

        if start_index != 0:
            print(rcv_frame_bytes[:start_index].decode("utf-8"))

        rcv_frame_bytes = rcv_frame_bytes[start_index : end_index + 1]
        print([hex(byte) for byte in rcv_frame_bytes])
        rcv_frame_bytes = ax25_proto.unstuff(rcv_frame_bytes)
        rcv_frame = ax25_proto.decode_frame(rcv_frame_bytes)
        print(rcv_frame.control.frame_type)

    # time.sleep(0.1)
    # cmd_to_send = create_cmd_downlink_logs_next_pass(1)
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_ping()
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_rtc_sync(1748380111)
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_mirco_sd_format()
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_downlink_telem()
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    time.sleep(0.1)
    cmd_to_send = create_cmd_uplink_disc()
    send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # send_command(cmd_ping, "/dev/ttyUSB0")
    fec_coder = FEC()
    fec_coder.destroy()
