import React from "react";
import {
  telemetryTypes,
  telemetrySubtypes,
  telemetryData,
  type TelemetryDataType,
} from "../../utils/mockTelemetryData";
import TypeSelector from "./components/TypeSelector";
import SubtypeSelector from "./components/SubtypeSelector";
import Table from "../../components/Table";
import { createColumnHelper } from "@tanstack/react-table";
import TelemetryChart from "./components/TelemetryChart";
import ThesholdSelector from "./components/ThesholdSelector";

const columnHelper = createColumnHelper<TelemetryDataType>();

const columns = [
  columnHelper.accessor("session", {
    header: "Session",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("telemetry", {
    header: "Telemetry",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("type", {
    header: "Type",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("packet", {
    header: "Packet",
    cell: (info) => info.getValue(),
  }),
];

function Telemetry() {
  const [type, setType] = React.useState(`${telemetryTypes[0]}`);
  const [subTypeList, setSubTypeList] = React.useState<string[]>(
    telemetrySubtypes[type]
  );
  const [threshold, setThreshold] = React.useState<string | number>(0);
  const [selectedSubTypeList, setSelectedSubTypeList] =
    React.useState<string[]>(subTypeList);

  function handleCommandSelect(row: TelemetryDataType) {
    console.log("Selected Telemetry Row:", row);
  }

  const selectedData = telemetryData[type].filter((row) => selectedSubTypeList.includes(row.type));

  return (
    <div className="mt-30 mx-8">
      <div className="gap-x-6 flex items-center justify-start">
        <TypeSelector
          type={type}
          setType={setType}
          setSubType={setSubTypeList}
          setSelectedSubTypeList={setSelectedSubTypeList}
        />
        <ThesholdSelector threshold={threshold} setThreshold={setThreshold} />
      </div>
      <div className="flex my-10 gap-x-7">
        <SubtypeSelector
          subTypeList={subTypeList}
          selectedSubTypeList={selectedSubTypeList}
          setSelectedSubTypeList={setSelectedSubTypeList}
        />
        <div className="min-w-2/5 max-w-1/2">
          <Table
            data={selectedData}
            columns={columns}
            onRowClick={handleCommandSelect}
            showFilters={true}
          />
        </div>
        <TelemetryChart
          type={type}
          telemetryData={selectedData}
        />
      </div>
    </div>
  );
}

export default Telemetry;
