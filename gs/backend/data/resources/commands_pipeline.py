import warnings

from gs.backend.data.resources.cli_commands import CLICommand


class CommandsPipeline:
    """
    Recieves, sorts, and packets commands such that they may be sent to the
    satellite.
    """

    def __init__(self) -> None:
        """
        Lockout should be set at some arbitrary time before session begins.
        Once lockout is True, commands will no longer be recieved
        """
        self.lockout = False
        self.commands_queue = []
        self.packet_list = []

    def command_to_byte(self) -> None:
        """
        Given any cli command, it converts it to a byte. We want to use this
        for sizing the packet, and as a helper function for packets
        """

        pass

    def queue_to_packet(self) -> None:
        """
        Converts all commands in the queue into packets.
        """

        # I am not completely sure how the commands are packed on a technical front so
        # I will consider two cases.
        # Case 1: We can convert bytes (which are commnads) into packets.
        # Case 2: We must convert cli commands into packets by passing a list of IDs

        # Case 1 we should just be able to apply the command to byte function to
        # every CLI command in the queue, then we can iterate over the queue again
        # to pack everything into packets (we know exactly when to stop for packets since we know size)
        # of the command

        # Case 2 uhh honestly im not sure but i think it involves a helper function which
        # takes a list of commands and packs them all into a packet, then checks size.
        # if the size still permits more packets, add one more command to the list of commands
        # to be packed and repack. repeat that until packet is at desired size.
        # if the size reached desired size, store the packet into packet_list.
        # note that the commands to be reference will likely be the queue.
        # so the helper function arguments be like [cmd1] -> [cmd1, cmd2] -> [cmd1 cmd 2 cmd3]
        # etc until desired size is reached in which we have [packet1], [cmdN] -> [cmdN+1] etc

        self.clear_queue()
        # this should return a list of packets, but since packet type is currently
        # undefined / unspecified, it will be casted to none.
        pass

    def add_to_queue(self, command: CLICommand) -> tuple[bool, list[CLICommand]]:
        """
        Inserts a command into queue, and then sorts it by priority and then by time.


        :command: CLICommand which has been rehydrated with the relevant infromation
        :return: tuple containing a status which is True if command has been inserted and
                 False otherwise and the command queue
        """

        if self.lockout:
            warnings.warn("Commands queue lockout active. Returned current queue", stacklevel=2)
            return tuple(False, self.commands_queue)

        self.commands_queue.append(command)
        self.sort_queue()

        return tuple(True, self.commands_queue)

    def sort_queue(self) -> list[CLICommand]:
        """
        This function sorts the queue 2 times. We first sort by time to ensure time descending,
        then we sort by priority to ensure that the highest priority is at the top of the
        queue.
        """
        self.commands_queue.sort(key=lambda x: x.time)
        self.commands_queue.sort(key=lambda x: x.prio)
        return self.commands_queue

    def apply_rule(self) -> list[CLICommand]:
        """
        There are certain rules which commands must follow.
        For example, some commands CANNOT come before other commands.
        These rules can be defined and applied to the current queue
        """
        return self.commands_queue

    def clear_queue(self) -> list[CLICommand]:
        """
        Clears the current queue
        """
        self.commands_queue = []
        return self.commands_queue

    def enable_lockout(self) -> None:
        """
        Prevents more commands from being recieved.
        """
        self.lockout = True

    def disable_lockout(self) -> None:
        """
        Allows more commands to be recieved
        """
        self.lockout = False
