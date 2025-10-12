import { useAppSelector } from "@/store/hooks";
import Table from "./components/Table"
import SelectCommand from "./components/SelectCommand"
import { selectCommand } from "./features/selectCommandSlice"
import SendCommand from "./components/SendCommand";

function Commands() {
  const selectedCommand = useAppSelector(selectCommand);
  return (
    <div className="min-h-screen w-full flex justify-center items-center space-x-10">
      {selectedCommand != "" && <SendCommand />}
      <Table />
      <SelectCommand />
    </div>
  )
}

export default Commands
