from cmd import Cmd

from interfaces.obc_gs_interface.commands import CmdCallbackId


class GroundStationShell(Cmd):
    """
    Cmd Class for the Ground Station Shell
    """

    intro = "Welcome to the UW Orbital Command Line Inteface! Type help of ? to list commands. \n"
    prompt = "(UW Orbital): "
    file = None

    def do_send_command(self, args: str) -> None:
        """
        Sends a command to the ground station
        """
        match int(args):
            case CmdCallbackId.CMD_END_OF_FRAME:
                print("0")
            case CmdCallbackId.CMD_EXEC_OBC_RESET:
                print("1")
            case _:
                print("Invalid Command")

    def do_exit(self, arg: str) -> None | bool:
        "Close the UW Orbital CLI"
        print("Closing CLI...")
        return True


if __name__ == "__main__":
    GroundStationShell().cmdloop()
