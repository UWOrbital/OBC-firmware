import sys
from cmd import Cmd
from datetime import datetime
from multiprocessing import Process

from serial import Serial, SerialException

from gs.backend.obc_utils.command_utils import arg_parse, poll, send_command, send_conn_request


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
        with open("obc_utils/logs.txt", "w") as file:
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
            send_conn_request(self._com_port)
        except IndexError:
            if self.background_logging is not None and not self.background_logging.is_alive():
                self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
                self.background_logging.start()
            print("Connection request was not successful. Try resetting the board")
            return
        else:
            if self.background_logging is not None and not self.background_logging.is_alive():
                self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
                self.background_logging.start()
            self._conn_request_sent = True

    def do_send_command(self, line: str) -> None:
        """
        Sends a command to the ground station
        """
        if not self._com_port:
            print("Com port needs to be configured using set_comm_port. Aborting...")
            return
        if not self._conn_request_sent:
            print("Connection Request needs to be sent first. Aborting...")
            return

        if self.background_logging is not None:
            self.background_logging.kill()

        send_command(line, self._com_port)

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
        if not self._com_port:
            print("Com port needs to be configured using set_comm_port. Aborting...")
            return
        if self.background_logging is None or not self.background_logging.is_alive():
            self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
            self.background_logging.start()
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
        if not self._com_port:
            print("Com port needs to be configured using set_comm_port. Aborting...")
            return

        with open("obc_utils/logs.txt") as file:
            print(file.read())

        if self.background_logging is not None:
            self.background_logging.kill()

        try:
            poll(self._com_port, True)
        except KeyboardInterrupt:
            print("Exiting Polling")

        if self.background_logging is not None and not self.background_logging.is_alive():
            self.background_logging = Process(target=poll, args=(self._com_port,), daemon=True)
            self.background_logging.start()

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
