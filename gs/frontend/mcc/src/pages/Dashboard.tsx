import {
  useReactTable,
  getCoreRowModel,
  flexRender,
  createColumnHelper,
  getSortedRowModel,
  getFilteredRowModel,
} from "@tanstack/react-table";
import { useState } from "react";

type DashboardData = {
  session: string;
  type: string;
  timestamp: string;
  packet: string;
};

const columnHelper = createColumnHelper<DashboardData>();

const columns = [
  columnHelper.accessor("session", {
    header: "Session",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("type", {
    header: "Type",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("timestamp", {
    header: "Timestamp",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("packet", {
    header: "Packet",
    cell: (info) => info.getValue(),
  }),
];

// Demo data
const data: DashboardData[] = [
  {
    session: "Session 1",
    type: "Telemetry",
    timestamp: "2025-10-11 14:23:45",
    packet: "0x4A2F",
  },
  {
    session: "Session 2",
    type: "Command",
    timestamp: "2025-10-11 14:25:12",
    packet: "0x3B1E",
  },
  {
    session: "Session 3",
    type: "Telemetry",
    timestamp: "2025-10-11 14:27:33",
    packet: "0x5C4D",
  },
  {
    session: "Session 1",
    type: "Status",
    timestamp: "2025-10-11 14:30:01",
    packet: "0x6D5E",
  },
];

/**
 * @brief Dashboard component displaying the dashboard table
 * @return tsx element of Dashboard component
 */
function Dashboard() {
  const [columnFilters, setColumnFilters] = useState("");
  const [globalFilter, setGlobalFilter] = useState("");

  const table = useReactTable({
    data,
    columns,
    getCoreRowModel: getCoreRowModel(),
    getSortedRowModel: getSortedRowModel(),
    getFilteredRowModel: getFilteredRowModel(),
    state: {
      globalFilter,
    },
    onGlobalFilterChange: setGlobalFilter,
  });

  return (
    <div className="flex flex-col items-center justify-center min-h-screen px-4 pt-32">
      <div className="w-full max-w-5xl">
        {/* Filter Section */}
        <div className="flex justify-between mb-6 gap-4">
          <input
            type="text"
            placeholder="Filter a column..."
            value={columnFilters}
            onChange={(e) => setColumnFilters(e.target.value)}
            className="flex-1 bg-gray-800/50 text-white px-4 py-2 rounded-lg border border-gray-600/50 focus:outline-none focus:border-gray-500"
          />
          <input
            type="text"
            placeholder="Filtered item name"
            value={globalFilter ?? ""}
            onChange={(e) => setGlobalFilter(e.target.value)}
            className="flex-1 bg-gray-800/50 text-white px-4 py-2 rounded-lg border border-gray-600/50 focus:outline-none focus:border-gray-500"
          />
        </div>

        {/* Table */}
        <div className="bg-gray-900/30 backdrop-blur-sm rounded-lg overflow-hidden border border-gray-700/50">
          <table className="w-full text-white">
            <thead>
              {table.getHeaderGroups().map((headerGroup) => (
                <tr
                  key={headerGroup.id}
                  className="border-b border-gray-700/50"
                >
                  {headerGroup.headers.map((header) => (
                    <th
                      key={header.id}
                      className="text-left px-6 py-4 font-normal text-gray-300"
                    >
                      {header.isPlaceholder
                        ? null
                        : flexRender(
                            header.column.columnDef.header,
                            header.getContext()
                          )}
                    </th>
                  ))}
                </tr>
              ))}
            </thead>
            <tbody>
              {table.getRowModel().rows.map((row) => (
                <tr
                  key={row.id}
                  className="border-b border-gray-800/50 hover:bg-gray-800/30 transition-colors"
                >
                  {row.getVisibleCells().map((cell) => (
                    <td key={cell.id} className="px-6 py-4 text-gray-200">
                      {flexRender(
                        cell.column.columnDef.cell,
                        cell.getContext()
                      )}
                    </td>
                  ))}
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}

export default Dashboard;
