import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select"

import { mockCommandsList } from "../../../../utils/mock-data.ts" // temporary commands list
import type { Command } from "../../../../utils/models.ts"
import { selectCommand, setCommand } from "../selectCommandSlice.ts";
import { useAppDispatch, useAppSelector } from "@/store/hooks.ts";

function SelectCommand() {
  const dispatch = useAppDispatch();
  const selectedCommand = useAppSelector(selectCommand);

  return (
    // change select command to be a small round button at the left bottom corner
    <Select value={selectedCommand} onValueChange={(value) => dispatch(setCommand(value))}>
        <SelectTrigger className="w-[180px]">
            <SelectValue placeholder="Select Command" />
        </SelectTrigger>
        <SelectContent>
            {mockCommandsList.map((command: Command) => (
                <SelectItem key={command.id} value={command.name}>{command.name}</SelectItem>
            ))}
        </SelectContent>
    </Select>
  )
}

export default SelectCommand
