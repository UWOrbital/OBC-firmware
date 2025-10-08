import { useAppSelector } from "@/store/hooks";
import SideModal from "./components/SideModal"
import Table from "./components/Table"
import SelectCommand from "./features/components/SelectCommand"
import { selectCommand } from "./features/selectCommandSlice"

function Commands() {
  const selectedCommand = useAppSelector(selectCommand);
  return (
    <div className="min-h-screen w-full flex justify-center items-center space-x-10">
      {selectedCommand != "" && <SideModal />}
      <Table />
      <SelectCommand />
    </div>
  )
}

export default Commands
