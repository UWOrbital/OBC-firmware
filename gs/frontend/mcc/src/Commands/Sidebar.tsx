import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select"

import { mockCommandsList } from "../utils/mock-data.ts" // temporary commands list
import type { Command } from "../utils/models.ts"

function Sidebar() {
  return (
    <div>
        <Select>
            <SelectTrigger className="w-[180px]">
                <SelectValue placeholder="Select Command" />
            </SelectTrigger>
            <SelectContent>
                {mockCommandsList.map((command: Command) => (
                    <SelectItem key={command.id} value={command.name}>{command.name}</SelectItem>
                ))}
            </SelectContent>
        </Select>
    </div>
  )
}

export default Sidebar
