from cmd import Cmd
from collections.abc import Iterable


class GroundStationShell(Cmd):
    """
    Cmd Class for the Ground Station Shell
    """

    intro = "Welcome to the UW Orbital Command Line Inteface! Type help of ? to list commands. \n"
    prompt = "(UW Orbital): "
    file = None

    def do_print(self, arg: Iterable) -> None | bool:
        "Print to Console"
        print("Hallo")

    def do_send_end_frame_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("End of CMD")

    def do_execute_obc_reset_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Reset the OBC")

    def do_rtc_sync_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Syncing RTC")

    def do_downlink_logs_next_pass_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Downlink Logs will be sent next pass")

    def do_mirco_sd_format_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Formatting SD Card")

    def do_ping_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Pinging")

    def do_downlink_telem_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Downlinking")

    def do_uplink_disconnect_cmd(self, arg: Iterable) -> None | bool:
        " "
        print("Disconnecting from uplink")

    def do_exit(self, arg: Iterable) -> None | bool:
        "Close the UW Orbital CLI"
        print("Closing CLI...")
        return True


if __name__ == "__main__":
    GroundStationShell().cmdloop()
