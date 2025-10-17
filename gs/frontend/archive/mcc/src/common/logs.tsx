import Table from "react-bootstrap/Table";
import { useQuery } from '@tanstack/react-query';

function LogItem({ log }: { log: any }) {
  return (
    <tr>
      <td>{log.date}</td>
      <td>{log.log}</td>
    </tr>
  );
}

function Logs() {
  const { data, isLoading} = useQuery({
    queryKey: ["logData"],
    queryFn: async () => {
      const response = await fetch("http://localhost:8000/api/v1/mcc/logs/");
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    },
  });

  return (
    <div className="logs layout">
      <Table responsive="sm">
        <thead>
          <tr>
            <th>Time</th>
            <th>Log</th>
          </tr>
        </thead>
        <tbody>
          {isLoading ? (
            <tr><td colSpan={2}>Loading...</td></tr>
          ) : (
            data?.map((log: any, idx: number) => <LogItem key={idx} log={log} />)
          )}
        </tbody>
      </Table>
    </div>
  );
}

export default Logs;
