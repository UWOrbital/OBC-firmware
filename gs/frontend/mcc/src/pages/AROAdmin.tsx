import {
  useReactTable,
  getCoreRowModel,
  flexRender,
  createColumnHelper,
  getSortedRowModel,
  getFilteredRowModel,
} from "@tanstack/react-table";
import { useState } from "react";

type AROData = {
  session: string;
  request: string;
  status: string;
  priority: string;
  location: string;
};

const columnHelper = createColumnHelper<AROData>();

const columns = [
  columnHelper.accessor("session", {
    header: "Session",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("request", {
    header: "Request",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("status", {
    header: "Status",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("priority", {
    header: "Priority",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("location", {
    header: "Location",
    cell: (info) => info.getValue(),
  }),
];

// Demo data
const data: AROData[] = [
  {
    session: "Session 1",
    request: "Image Capture",
    status: "Pending",
    priority: "High",
    location: "47.5°N, 122.3°W",
  },
  {
    session: "Session 2",
    request: "Thermal Scan",
    status: "Processing",
    priority: "Medium",
    location: "34.0°N, 118.2°W",
  },
  {
    session: "Session 3",
    request: "Spectral Analysis",
    status: "Completed",
    priority: "Low",
    location: "40.7°N, 74.0°W",
  },
  {
    session: "Session 4",
    request: "Video Recording",
    status: "Failed",
    priority: "High",
    location: "51.5°N, 0.1°W",
  },
];

/**
 * @brief AROAdmin component displaying the ARO admin table
 * @return tsx element of AROAdmin component
 */
function AROAdmin() {
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

export default AROAdmin;
