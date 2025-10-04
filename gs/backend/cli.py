from textual.app import App, ComposeResult
from textual.containers import ScrollableContainer, VerticalScroll, HorizontalGroup
from textual.widgets import Static, Input, Button, Label
from textual.reactive import reactive
import io, sys
from sys import argv
from gs.backend.ground_station_cli import GroundStationShell

COM_PORT = argv[1]

class CliPanel(Static):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        shell = GroundStationShell(COM_PORT)

        #  buf creates a StringIO instance, which will store what is printed from the cli through a redirection of stdout (default is console)
        buffer = io.StringIO()

        #  redirects the output stream (sys.stdout) to buf, meaning that anything printed will now be written to buf
        sys.stdout = buffer

        if shell.intro is not None:
            print(shell.intro)
        print(shell.prompt, end="")

        # buffer.getvalue() returns the contents of the string buffer as a str
        self.intro_text = buffer.getvalue()

    def compose(self) -> ComposeResult:
        self.update("CLI\n" + self.intro_text)
        yield Input(placeholder="Enter command here:")


class CmdButton(HorizontalGroup):
    def __init__(self, cmdname):
        super().__init__()
        self.cmdname = cmdname
    
    def compose(self) -> ComposeResult:
        yield Label(f"{self.cmdname}", id="button-label")
        yield Button("Run")


class LogsPanel(Static):
    logs = reactive("")

    def on_mount(self) -> None:
        self.logs_refresh = self.set_interval(1 / 60, self.update_logs)

    def update_logs(self) -> None:
        try:
            with open("gs/backend/logs.txt") as logs:
                self.logs = logs.read()
        except FileNotFoundError:
            print("File unable to be found")
        print(self.logs)

    def watch_logs(self, logs: str):
        self.update("LOGS\n\n" + self.logs)

class CLIWindow(App):
    CSS_PATH = "cli.tcss"

    def compose(self) -> ComposeResult:
        yield ScrollableContainer(LogsPanel("LOGS"), can_focus=True, id="logs")
        yield CliPanel(id="cli")
        yield VerticalScroll(Static("CMDS"), CmdButton("print_logs"), CmdButton("send_conn_request"), CmdButton("start_logging"), id="cmd-panel")
        yield Static("MISC", id="misc2")

def main() -> None:
    if len(argv) != 2:
        print("One argument needed: Com Port")
        return

    app = CLIWindow()
    app.run()

if __name__ == "__main__":
    main()
