// src/components/users/UserTable.tsx
"use client";

import { useState } from "react";

import type { User } from "./Columns";
import { columns } from "./Columns";

import type {
  ColumnFiltersState,
  SortingState,
} from "@tanstack/react-table";

import {
  getPaginationRowModel,
  getFilteredRowModel,
  getSortedRowModel,
  useReactTable,
  getCoreRowModel,
} from "@tanstack/react-table";

import AroAdminTable from "@/components/AroAdminTable";
import { Button } from "@/components/ui/button";
import * as XLSX from "xlsx";

function getPageNumbers(pageIndex: number, pageCount: number) {
    const pages: (number | "...")[] = [];
    const maxDisplay = 5;

    if (pageCount <= maxDisplay) {
        return [...Array(pageCount)].map((_, i) => i);
    }

    const first = 0;
    const last = pageCount - 1;

    pages.push(first);

    if (pageIndex > 2) pages.push("...");

    const start = Math.max(1, pageIndex - 1);
    const end = Math.min(pageCount - 2, pageIndex +1 );
    for (let i = start; i <= end; ++i) {
        pages.push(i);
    }

    if (pageIndex < pageCount - 3) pages.push("...");

    pages.push(last);

    return pages;
}

export default function UserTable({ data }: { data: User[] }) {
  const [sorting, setSorting] = useState<SortingState>([]);
  const [columnFilters, setColumnFilters] = useState<ColumnFiltersState>([]);
  const [globalFilter, setGlobalFilter] = useState("");
  const [pagination, setPagination] = useState({
    pageIndex: 0,
    pageSize: 10,
  })

  const table = useReactTable({
    data,
    columns,
    state: {
      sorting,
      globalFilter,
      columnFilters,
      pagination,
    },
    onSortingChange: setSorting,
    onGlobalFilterChange: setGlobalFilter,
    onColumnFiltersChange: setColumnFilters,
    onPaginationChange: setPagination,

    getCoreRowModel: getCoreRowModel(),
    getSortedRowModel: getSortedRowModel(),
    getFilteredRowModel: getFilteredRowModel(),
    getPaginationRowModel: getPaginationRowModel(),
  });

  const exportToXlsx = () => {
    const rows = table.getFilteredRowModel().rows.map((r) => r.original);
    const worksheet = XLSX.utils.json_to_sheet(rows);
    const workbook = XLSX.utils.book_new();

    XLSX.utils.book_append_sheet(workbook, worksheet, "Users");
    XLSX.writeFile(workbook, "users.xlsx");
  };

  return (
    <div className="flex flex-col items-center px-16 pt-16 pb-16">

      <div className="flex justify-between mb-4 gap-4">
        <input
          className="px-4 py-2 bg-white/10 rounded-lg w-64"
          placeholder="Search"
          value={globalFilter}
          onChange={(e) => setGlobalFilter(e.target.value)}
        />

        {/* Role Filter */}
        <select
          className="px-4 py-2 bg-white/10 rounded-lg"
          onChange={(e) =>
            table.getColumn("role")?.setFilterValue(e.target.value || undefined)
          }
        >
          <option value="">All Roles</option>
          <option value="Admin">Admin</option>
          <option value="User">User</option>
        </select>

        <Button onClick={exportToXlsx}>Export</Button>
        <Button >Add User</Button>
      </div>

    <div className="w-full max-h-[600px] overflow-y-auto rounded-lg">
      <AroAdminTable<User>
      data={table.getRowModel().rows.map((r) => r.original)}
      columns={columns}
      showFilters={false}
      containerClassName="w-full"
    />
    </div>

      <div className="flex justify-center gap-2 mt-4">

        {/* Numbered pagination */}
        {getPageNumbers(
          pagination.pageIndex,
          table.getPageCount()
        ).map((p, i) =>
          p === "..." ? (
            <span key={i} className="px-2 text-gray-400">…</span>
          ) : (
            <button
              key={i}
              className={
                "px-3 py-1 rounded " +
                (p === pagination.pageIndex
                  ? "bg-blue-600 text-white"
                  : "bg-white/10 text-gray-300")
              }
              onClick={() => table.setPageIndex(p)}
            >
              {p + 1}
            </button>
          )
        )}
      </div>
    </div>
  );
}
