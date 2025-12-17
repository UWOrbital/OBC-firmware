import { createColumnHelper } from "@tanstack/react-table"
import { Pencil, Trash } from "lucide-react";
import { Button } from "@/components/ui/button";

export type User = {
    id: string;
    username: string;
    fullName: string;
    email: string;
    dateCreated: string;
    callsign: string;
    role: string;
}

const columnHelper = createColumnHelper<User>();

export const columns = [
    columnHelper.display({
        id: "select",
        header: () => <input type="checkbox" />,
        cell: () => <input type="checkbox" />,
    }),

    columnHelper.accessor("username", {
        header: "Username",
        enableSorting: true,
    }),

    columnHelper.accessor("email", {
        header: "Email",
        enableSorting: true,
    }),

    columnHelper.accessor("dateCreated", {
        header: "Date Created",
        enableSorting: true,
        sortingFn: (a, b, columnId) => {
            const dateA = new Date(a.getValue(columnId));
            const dateB = new Date(b.getValue(columnId));

            return dateA.getTime() - dateB.getTime();
        },
        cell: ({ getValue }) => {
            const value = getValue();
            return <span>{value}</span>;
        }
    }),

    columnHelper.accessor("callsign", {
        header: "Callsign",
        enableSorting: true,
        cell: ({ getValue }) => {
        const value = getValue();
        const unverified = value === "Unverified";
        return (
            <span
            className={`px-2 py-1 rounded text-sm ${
                unverified ? "bg-red-700/40 text-red-300" : "bg-green-700/40 text-green-300"
            }`}
            >
            {value}
            </span>
        );
        },
    }),

    columnHelper.accessor("role", {
        header: "Role",
        enableSorting: true,
    }),

    columnHelper.display({
        id: "action",
        header: "Actions",
        cell: ({ row }) => {
            return (
                <div className="flex gap-2">
                <Button size="icon" variant="ghost" onClick={() => console.log("edit", row.original)}>
                    <Pencil size={14} />
                </Button>
                <Button size="icon" variant="ghost" onClick={() => console.log("delete", row.original)}>
                    <Trash size={14} className="text-red-400" />
                </Button>
                </div>
            );
        },
    }),

]
