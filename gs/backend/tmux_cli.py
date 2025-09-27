import libtmux
import time
from libtmux.constants import PaneDirection
from sys import argv


def main() -> None:
    """
    Set up a tmux session with two panes: one for logs, one for the CLI.
    """
    if len(argv) != 2:
        print("One argument needed: Com Port")
        return

    com_port = str(argv[1])
    server = libtmux.Server()
    if server.has_session("gs_cli"):
        server.kill_session("gs_cli")

    # Does not attach the new session right away through attach parameter
    session = server.new_session(session_name="gs_cli", attach=False)
    window = session.active_window

    pane1 = window.active_pane
    pane2 = window.split(attach=True, direction=PaneDirection.Right, size=80)

    # Initialize gs cli
    pane2.send_keys(
        "source .venv/bin/activate && "
        "tmux source-file .tmux.conf && "
        f"python3.11 gs/backend/ground_station_cli.py {com_port}"
    )

    # Sleep to allow appropriate time for cli setup
    time.sleep(1.5)

    # Enable logs pane
    pane1.send_keys(f"tail -f gs/backend/logs.log")

    # Display tmux cli
    session.attach()


if __name__ == "__main__":
    main()
