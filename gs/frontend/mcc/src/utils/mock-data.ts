import type { Command } from "./models";

const mockCommandsList: Command[] = [
  {
    id: 1,
    name: "RTC Sync",
    params: "time",
    format: "int 7 bytes",
    data_size: 7,
    total_size: 8,
  },
  {
    id: 2,
    name: "Manually activate an emergency mode for a specified amount of time",
    params: "mode_state_number,time",
    format: "int 1 byte, int 7 bytes",
    data_size: 8,
    total_size: 9,
  },
];

export { mockCommandsList };