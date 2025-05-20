from cmd import Cmd

from comms_pipeline import arg_parse


class GroundStationShell(Cmd):
    """
    Cmd Class for the Ground Station Shell
    """

    def __init__(self) -> None:
        Cmd.__init__(self)
        self.parser = arg_parse()

    intro = "Welcome to the UW Orbital Command Line Inteface! Type help of ? to list commands. \n"
    prompt = "(UW Orbital): "
    file = None

    def do_send_command(self, line: str) -> None:
        """
        Sends a command to the ground station
        """
        try:
            parsed = self.parser.parse_args(line.split())
            print(parsed)
        except SystemExit:
            print("Try again")
        # TODO: Add a function that sends to other file

    def do_exit(self, arg: str) -> None | bool:
        "Close the UW Orbital CLI"
        print("Closing CLI...")
        return True


if __name__ == "__main__":
    GroundStationShell().cmdloop()
