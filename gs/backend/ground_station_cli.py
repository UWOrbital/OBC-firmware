import sys
from cmd import Cmd
from datetime import datetime
from multiprocessing import Process

from serial import Serial, SerialException

from gs.backend.comms_pipeline import arg_parse, poll, send_command, send_conn_request
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    create_cmd_downlink_telem,
    create_cmd_end_of_frame,
    create_cmd_exec_obc_reset,
    create_cmd_mirco_sd_format,
    create_cmd_ping,
    create_cmd_rtc_sync,
    create_cmd_uplink_disc,
)


class GroundStationShell(Cmd):
    """
    Cmd Class for the Ground Station Shell
    """

    def __init__(self) -> None:
        Cmd.__init__(self)
        self.parser = arg_parse()
        self._com_port: str = ""
        self._conn_request_sent: bool = False
        self._verbose: bool = False
        self.background_logging: Process | None = None
        with open("logs.txt", "w") as file:
            file.write("LOGS (Date: " + str(datetime.now()) + ")\n")

    intro = """

    ██╗   ██╗██╗    ██╗     ██████╗ ██████╗ ██████╗ ██╗████████╗ █████╗ ██╗
    ██║   ██║██║    ██║    ██╔═══██╗██╔══██╗██╔══██╗██║╚══██╔══╝██╔══██╗██║
    ██║   ██║██║ █╗ ██║    ██║   ██║██████╔╝██████╔╝██║   ██║   ███████║██║
    ██║   ██║██║███╗██║    ██║   ██║██╔══██╗██╔══██╗██║   ██║   ██╔══██║██║
    ╚██████╔╝╚███╔███╔╝    ╚██████╔╝██║  ██║██████╔╝██║   ██║   ██║  ██║███████╗
     ╚═════╝  ╚══╝╚══╝      ╚═════╝ ╚═╝  ╚═╝╚═════╝ ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝
  Welcome to the UW Orbital Command Line Inteface! Type help of ? to list commands.\n
    """
    prompt = "(UW Orbital): "
    file = None

    def do_send_conn_request(self, line: str) -> None:
        "Sends the initial connection request"
        if not self._com_port:
            print("Com port needs to be configured using set_comm_port. Aborting...")
            return
        if self._conn_request_sent:
            print("Connection Request has already been sent. Aborting...")
            return

        try:
            if self.background_logging is not None:
                self.background_logging.kill()
            send_conn_request(self._com_port, self._verbose)
            if self.background_logging is not None:
                self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
                self.background_logging.start()
        except IndexError:
            if self.background_logging is not None and not self.background_logging.is_alive():
                self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
                self.background_logging.start()
            print("Connection request was not successful. Try resetting the board")
            return
        self._conn_request_sent = True

    def do_send_command(self, line: str) -> None:
        """
        Sends a command to the ground station
        """
        try:
            parsed = self.parser.parse_args(line.split())
        except SystemExit:
            if line.split()[0] != "-h" and line.split()[0] != "--help":
                print("Invalid input into console")
        else:
            if not self._com_port:
                print("Com port needs to be configured using set_comm_port. Aborting...")
                return
            if not self._conn_request_sent:
                print("Connection Request needs to be sent using send_conn_request. Aborting...")
                return

            command = create_cmd_end_of_frame()
            match parsed.command:
                case str(CmdCallbackId.CMD_PING.name):
                    command = create_cmd_ping(parsed.timestamp)
                case str(CmdCallbackId.CMD_EXEC_OBC_RESET.name):
                    command = create_cmd_exec_obc_reset()
                case str(CmdCallbackId.CMD_RTC_SYNC.name):
                    if parsed.rtc_sync is None:
                        print("Unixtime not provided for RTC Sync. Aborting...")
                        return
                    command = create_cmd_rtc_sync(parsed.rtc_sync, parsed.timestamp)
                case str(CmdCallbackId.CMD_DOWNLINK_LOGS_NEXT_PASS.name):
                    if parsed.log_level is None:
                        print("Log Level not provided. Aborting...")
                        return
                    command = create_cmd_rtc_sync(parsed.log_level, parsed.timestamp)
                case str(CmdCallbackId.CMD_MICRO_SD_FORMAT.name):
                    command = create_cmd_mirco_sd_format(parsed.timestamp)
                case str(CmdCallbackId.CMD_PING.name):
                    command = create_cmd_ping(parsed.timestamp)
                case str(CmdCallbackId.CMD_DOWNLINK_TELEM.name):
                    command = create_cmd_downlink_telem(parsed.timestamp)
                case str(CmdCallbackId.CMD_UPLINK_DISC.name):
                    command = create_cmd_uplink_disc(parsed.timestamp)
                case _:
                    print("Invalid Command to Send")

            if self.background_logging is not None:
                self.background_logging.kill()

            send_command(command, self._com_port, self._verbose)

            if self.background_logging is not None and not self.background_logging.is_alive():
                self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
                self.background_logging.start()

    def do_set_comm_port(self, line: str) -> None:
        """
        Sets the comm port to be used to send commands
        """
        try:
            ser = Serial(line)
            print("Comm port set to: " + str(ser.name))
            ser.close()
        except SerialException:
            print("Invalid Port Entered")
            return
        else:
            self._com_port = line

    def do_start_logging(self, line: str) -> None:
        """
        Start writing logs from the board in a file
        """
        if self.background_logging is None or not self.background_logging.is_alive():
            self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
            self.background_logging.start()
            print("Started Logging")
        else:
            print("Logging has already been started")

    def do_set_verbosity(self, line: str) -> None:
        """
        Sets the verbosity of the commands used
        """
        args = line.lower()
        if args == "true":
            self._verbose = True
        elif args == "false":
            self._verbose = False
        else:
            print("Invalid Verbsoity Passed In")

    def do_print_logs(self, line: str) -> None:
        """
        Prints out logs
        """
        with open("logs.txt") as file:
            print(file.read())

    def do_set_conn(self, line: str) -> None:
        """
        Configures whether the connection request was already sent via a boolean
        """
        args = line.lower()
        if args == "true":
            self._conn_request_sent = True
        elif args == "false":
            self._conn_request_sent = False
        else:
            print("Invalid Bool Passed In")

    def help_send_command(self) -> None:
        """
        Help to send a command
        """
        self.parser.print_help()

    def do_exit(self, arg: str) -> None | bool:
        "Close the UW Orbital CLI"
        print("Closing CLI...")
        sys.exit()


if __name__ == "__main__":
    GroundStationShell().cmdloop()
