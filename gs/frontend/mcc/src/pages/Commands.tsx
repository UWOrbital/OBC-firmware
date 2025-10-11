import {
  useReactTable,
  getCoreRowModel,
  flexRender,
  createColumnHelper,
  getSortedRowModel,
  getFilteredRowModel,
} from "@tanstack/react-table";
import { useState } from "react";

type CommandData = {
  session: string;
  command: string;
  status: "Pending" | "Sent" | "Responded" | "Failed";
  type: string;
  parameters: string;
};

const columnHelper = createColumnHelper<CommandData>();

const columns = [
  columnHelper.accessor("session", {
    header: "Session",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("command", {
    header: "Command",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("status", {
    header: "Status",
    cell: (info) => {
      const status = info.getValue();
      const statusColors: Record<string, string> = {
        Pending: "text-yellow-400",
        Sent: "text-green-400",
        Responded: "text-blue-400",
        Failed: "text-red-400",
      };
      return (
        <span className={statusColors[status] || "text-gray-400"}>
          {status}
        </span>
      );
    },
  }),
  columnHelper.accessor("type", {
    header: "Type",
    cell: (info) => info.getValue(),
  }),
  columnHelper.accessor("parameters", {
    header: "Parameters",
    cell: (info) => info.getValue(),
  }),
];

// Demo data
const data: CommandData[] = [
  {
    session: "Session 1",
    command: "PING",
    status: "Pending",
    type: "Diagnostic",
    parameters: "{}",
  },
  {
    session: "Session 2",
    command: "SET_MODE",
    status: "Sent",
    type: "Configuration",
    parameters: '{"mode": "autonomous"}',
  },
  {
    session: "Session 3",
    command: "GET_STATUS",
    status: "Responded",
    type: "Query",
    parameters: "{}",
  },
  {
    session: "Session 4",
    command: "DEPLOY",
    status: "Failed",
    type: "Action",
    parameters: '{"component": "solar_panel_2"}',
  },
];

/**
 * @brief Commands component displaying the commands table with a sidebar
 * @return tsx element of Commands component
 */
function Commands() {
  const [columnFilters, setColumnFilters] = useState("");
  const [globalFilter, setGlobalFilter] = useState("");
  const [sidebarOpen, setSidebarOpen] = useState(false);
  const [selectedCommand, setSelectedCommand] = useState("");

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

  const handleCommandSelect = (command: string) => {
    setSelectedCommand(command);
  };

  return (
    <div className="flex min-h-screen pt-32">
      {/* Sidebar */}
      <div
        className={`fixed left-0 top-32 h-[calc(100vh-8rem)] bg-gray-900/50 backdrop-blur-sm border-r border-gray-700/50 transition-all duration-300 ${
          sidebarOpen ? "w-64" : "w-16"
        }`}
      >
        <button
          onClick={() => setSidebarOpen(!sidebarOpen)}
          className="w-full p-4 text-white hover:bg-gray-800/50 transition-colors text-left"
        >
          {sidebarOpen ? "◀" : "▶"}
        </button>
        {sidebarOpen && (
          <div className="p-4 space-y-2">
            <div className="text-white font-medium mb-4">
              {selectedCommand || "{Command name}"}
            </div>
            <input
              type="text"
              placeholder="input1"
              className="w-full bg-gray-800/50 text-white px-3 py-2 rounded border border-gray-600/50 focus:outline-none focus:border-gray-500 text-sm"
            />
            <input
              type="text"
              placeholder="input2"
              className="w-full bg-gray-800/50 text-white px-3 py-2 rounded border border-gray-600/50 focus:outline-none focus:border-gray-500 text-sm"
            />
            <input
              type="text"
              placeholder="input3"
              className="w-full bg-gray-800/50 text-white px-3 py-2 rounded border border-gray-600/50 focus:outline-none focus:border-gray-500 text-sm"
            />
          </div>
        )}
      </div>

      {/* Main Content */}
      <div
        className={`flex-1 px-4 transition-all duration-300 ${
          sidebarOpen ? "ml-64" : "ml-16"
        }`}
      >
        <div className="max-w-5xl mx-auto">
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
                    className="border-b border-gray-800/50 hover:bg-gray-800/30 transition-colors cursor-pointer"
                    onClick={() => handleCommandSelect(row.original.command)}
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
    </div>
  );
}

export default Commands;
