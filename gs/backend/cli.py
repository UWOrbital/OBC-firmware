from textual.app import App, ComposeResult
from textual.containers import ScrollableContainer, VerticalScroll, HorizontalGroup
from textual.widgets import Static, Input, Button, Label
from textual.reactive import reactive

class CliPanel(Static):
    def compose(self) -> ComposeResult:
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
        yield CliPanel("CLI", id="cli")
        # yield Input(compact=True)
        yield VerticalScroll(Static("CMDS"), CmdButton("print_logs"), CmdButton("send_conn_request"), CmdButton("start_logging"), id="cmd-panel")
        yield Static("MISC", id="misc2")


if __name__ == "__main__":
    app = CLIWindow()
    app.run()
