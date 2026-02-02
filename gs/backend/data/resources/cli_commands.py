from datetime import datetime


class CLICommand:
    """
    An abstraction of the CLI commands so that adapting the commns pipeline packet logic
    into GS will be easier.
    """

    def __init__(self, name: str, params: list[str], cmd_id: int, prio: int) -> None:
        """
        This abstracts the CLI commands in a way which makes it accessable for GS.
        The reason this is created is so that we are able to have a 1:1 clone of the
        CLI commands which allow for easier adoption of prexisting pipelines built for CLI.

        :name: name which matches the CLI command name
        :id: id which matches the id in the satelite
        :params: list of command as a string, matches CLI command param options
        :prio: command priority. integer which goes from 1 to n where n is the number of
               priorities we have. 1 is the highest priority
        :time: tracks the time at which a command has been created
        """
        self.name = name
        self.cmd_id = cmd_id
        self.params = params
        self.prio = prio
        self.time = datetime.now()
