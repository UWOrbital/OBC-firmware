from cmd import Cmd


class GroundStationShell(Cmd):
    """
    Cmd Class for the Ground Station Shell
    """

    _AVAILABLE_COMMANDS = (
        "CMD_END_OF_FRAME",
        "CMD_EXEC_OBC_RESET",
        "CMD_RTC_SYNC",
        "CMD_DOWNLINK_LOGS_NEXT_PASS",
        "CMD_MICRO_SD_FORMAT",
        "CMD_PING",
        "CMD_DOWNLINK_TELEM",
        "CMD_UPLINK_DISC",
    )

    intro = "Welcome to the UW Orbital Command Line Inteface! Type help of ? to list commands. \n"
    prompt = "(UW Orbital): "
    file = None

    def do_send_command(self, args: str) -> None:
        """
        Sends a command to the ground station
        """
        arguments = args.split()
        if arguments[0] != "CMD_RTC_SYNC" and arguments[0] != "CMD_DOWNLINK_LOGS_NEXT_PASS" and len(arguments) > 1:
            print("Omitting Extra Arguments")

        match arguments[0]:
            case "CMD_END_OF_FRAME":
                print("0")
            case "CMD_EXEC_OBC_RESET":
                print("1")
            case "CMD_RTC_SYNC":
                print("2")
            case "CMD_DOWNLINK_LOGS_NEXT_PASS":
                print("3")
            case "CMD_MICRO_SD_FORMAT":
                print("4")
            case "CMD_PING":
                print("5")
            case "CMD_DOWNLINK_TELEM":
                print("6")
            case "CMD_UPLINK_DISC":
                print("7")
            case _:
                print("Invalid Command")

    def complete_send_command(self, text: str, line: int, begidx: int, endidx: int) -> list[str] | None:
        """
        Autocomplete method
        """
        return [i for i in self._AVAILABLE_COMMANDS if i.startswith(text)]

    def do_exit(self, arg: str) -> None | bool:
        "Close the UW Orbital CLI"
        print("Closing CLI...")
        return True


if __name__ == "__main__":
    GroundStationShell().cmdloop()
