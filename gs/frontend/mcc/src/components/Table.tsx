import {
  useReactTable,
  getCoreRowModel,
  flexRender,
  getSortedRowModel,
  getFilteredRowModel,
} from "@tanstack/react-table";
import type { ColumnDef } from "@tanstack/react-table";
import { useState } from "react";

interface TableProps<T> {
  data: T[];
  columns: ColumnDef<T, any>[];
  onRowClick?: (row: T) => void;
  showFilters?: boolean;
  containerClassName?: string;
}

/**
 * @brief Reusable Table component with filtering and sorting capabilities
 * @param data - Array of data objects to display
 * @param columns - Column definitions using TanStack Table's ColumnDef
 * @param onRowClick - Optional callback when a row is clicked
 * @param showFilters - Whether to show filter inputs (default: true)
 * @param containerClassName - Optional custom className for the container
 * @return tsx element of Table component
 */
function Table<T>({
  data,
  columns,
  onRowClick,
  showFilters = true,
  containerClassName = "w-full max-w-5xl",
}: TableProps<T>) {
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
    <div className={containerClassName}>
      {/* Filter Section */}
      {showFilters && (
        <div className="flex justify-between mb-6 gap-4">
          <input
            type="text"
            placeholder="Search..."
            value={globalFilter ?? ""}
            onChange={(e) => setGlobalFilter(e.target.value)}
            className="flex-1 bg-gray-800/50 text-white px-4 py-2 rounded-lg border border-gray-600/50 focus:outline-none focus:border-gray-500"
          />
        </div>
      )}

      {/* Table */}
      <div className="bg-gray-900/30 backdrop-blur-sm rounded-lg overflow-hidden border border-gray-700/50">
        <table className="w-full text-white">
          <thead>
            {table.getHeaderGroups().map((headerGroup) => (
              <tr key={headerGroup.id} className="border-b border-gray-700/50">
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
                className={`border-b border-gray-800/50 hover:bg-gray-800/30 transition-colors ${
                  onRowClick ? "cursor-pointer" : ""
                }`}
                onClick={() => onRowClick?.(row.original)}
              >
                {row.getVisibleCells().map((cell) => (
                  <td key={cell.id} className="px-6 py-4 text-gray-200">
                    {flexRender(cell.column.columnDef.cell, cell.getContext())}
                  </td>
                ))}
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}

export default Table;
