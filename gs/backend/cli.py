from textual import on
from textual.app import App, ComposeResult
from textual.containers import ScrollableContainer, VerticalScroll, HorizontalGroup, HorizontalScroll, Container
from textual.widgets import Static, Input, Button, Label, DataTable
from textual.reactive import reactive
import io, sys
from sys import argv
from gs.backend.ground_station_cli import GroundStationShell
from gs.backend.obc_utils.command_utils import poll
from serial import Serial
import threading


COM_PORT = argv[1]
shell = GroundStationShell(COM_PORT)

class CliPanel(ScrollableContainer):
    cli_output = reactive("")
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.shell = shell
        self.cli_output_panel = None
        #  Hold the original output stream
        self.sys_stdout = sys.stdout

        #  Buf creates a StringIO instance, storing what is printed from the cli through a redirection of stdout
        self.buffer = io.StringIO()

        #  Redirects the output stream (sys.stdout) to buffer, meaning that anything printed will now be written to buffer
        sys.stdout = self.buffer

        #  Redirects the output stream of the gs shell to buffer, allowing help descriptions to be displayed
        self.shell.stdout = sys.stdout

        if self.shell.intro is not None:
            print(self.shell.intro)
    
    def on_mount(self) -> None:
        self.output_refresh = self.set_interval(1 / 600, self.update_cli)
        self.cli_output_panel = self.query_one("#cli-output-panel", Static)

        # Buffer.getvalue() returns the contents of the string buffer as a str
        self.cli_output = self.buffer.getvalue()
    
    def on_unmount(self) -> None:
        #  Upon exitting the cli, restore the original output stream
        sys.stdout = self.sys_stdout

    def watch_cli_output(self, cli_output: str): 
        if self.cli_output_panel:
           self.cli_output_panel.update(f"CLI - {COM_PORT}\n" + self.cli_output)

    def update_cli(self) -> None:
        self.cli_output = self.buffer.getvalue()
    
    #  Use threads to ensure blocking commands don't block CLI
    def run_cli_command_in_thread(self, cmd_function, args):
        thread = threading.Thread(
            target=cmd_function,
            args=(args,),
            daemon=True
        )
        thread.start()

    @on(Input.Submitted)
    def submit_command(self) -> None:
        cli_input = self.query_one(Input)
        print(f"(UW Orbital): {cli_input.value}")

        #  Splits the input into two parts: command name and args
        command_parts = cli_input.value.strip().split(maxsplit= 1)
        try: 
            # Obtain function from gs shell instance based on inputted command
            cmd_function = getattr(self.shell, f"do_{command_parts[0]}")
            if len(command_parts) > 1 and command_parts[1] is not None: 
                args = command_parts[1]
            else:
                args = ""

            # Make special exception for "exit" command
            if(command_parts[0] == "exit"):
                self.app.exit()
                return

            self.run_cli_command_in_thread(cmd_function, args)

        except (AttributeError, IndexError):
            print(f"*** Unknown syntax: {cli_input.value}")

        self.query_one(Input).value = ""

    def compose(self) -> ComposeResult:
        yield Static("", id="cli-output-panel")
        yield Input(placeholder="Enter command here:", id="cli-input")
        

class CmdButton(HorizontalGroup):
    def __init__(self, cmdname):
        super().__init__()
        self.cmdname = cmdname
        self.shell = shell

    def on_button_pressed(self, event: Button.Pressed) -> None:
        cmd_function = getattr(self.shell, f"do_{self.cmdname}")
        args = ""

        thread = threading.Thread(
            target=cmd_function,
            args=(args,),
            daemon=True
        )

        if event.button.label != "STOP":
            print(f"(UW Orbital): {self.cmdname}")
            thread.start()

            if self.cmdname != "print_logs":
                event.button.disabled = True

        if self.cmdname == "print_logs":
            if event.button.label == "Run":
                event.button.label = "STOP"
            else:
                event.button.label = "Run"
                self.shell.stop_printing = True

    def compose(self) -> ComposeResult:
        yield Label(f"{self.cmdname}", id="button-label")
        yield Button("Run")


class TimeTaggedLogs(HorizontalScroll):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.rows = [("CMD", "Time", "Cate1", "Cate2"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),
                     ("ABC", "ABC", "ABC", "ABC"),]
    
    def compose(self) -> ComposeResult:
        yield Label("TIME TAGGED CMDS")
        yield DataTable()

    def on_mount(self) -> None:
        table = self.query_one(DataTable)
        table.add_columns(*self.rows[0])
        table.add_rows(self.rows[1:])


class LogsPanel(Static):
    logs = reactive("")

    def on_mount(self) -> None:
        self.logs_refresh = self.set_interval(1 / 60, self.update_logs)

    def update_logs(self) -> None:
        try:
            with open("gs/backend/logs.log") as logs:
                self.logs = logs.read()
        except FileNotFoundError:
            print("File unable to be found")

    def watch_logs(self, logs: str):
        self.update("LOGS\n\n" + self.logs)


class CLIWindow(App):
    CSS_PATH = "cli.tcss"
    
    def on_mount(self) -> None:
        self.theme = "dracula"

    def compose(self) -> ComposeResult:
        yield ScrollableContainer(LogsPanel("LOGS"), can_focus=True, id="logs")
        yield CliPanel(id="cli")
        yield VerticalScroll(Static("CMDS"), CmdButton("print_logs"), CmdButton("send_conn_request"), CmdButton("start_logging"), id="cmd-panel")
        yield TimeTaggedLogs(id="timetagged")

def main() -> None:
    if len(argv) != 2:
        print("One argument needed: Com Port")
        return
    
    ser = Serial(COM_PORT)
    print("Comm port set to: " + str(ser.name))
    ser.close()
    
    app = CLIWindow()
    app.run()

if __name__ == "__main__":
    main()
