import { createColumnHelper } from "@tanstack/react-table";
import Table from "../components/Table";

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
 * @brief Commands component displaying the commands table
 * @return tsx element of Commands component
 */
function Commands() {
  const handleCommandSelect = (command: CommandData) => {
    // TODO: Handle command selection (e.g., show details in a modal)
    console.log("Selected command:", command.command);
  };

  return (
    <div className="flex flex-col items-center justify-center min-h-screen px-4 pt-32">
      <Table
        data={data}
        columns={columns}
        onRowClick={handleCommandSelect}
        showFilters={true}
      />
    </div>
  );
}

export default Commands;
