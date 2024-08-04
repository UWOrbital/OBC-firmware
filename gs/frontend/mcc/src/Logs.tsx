import Table from "react-bootstrap/Table";
import { useState, useEffect } from "react";

type LogItemProps = {
  id: number;
};
function LogItem({ id }: LogItemProps) {
  const [date, setDate] = useState(Date.now());
  const [log, setLog] = useState("");
  const [loading, setLoading] = useState(true)

  const fetchLogData = async () => {
    const response = await fetch(`http://localhost:5000/recent-logs/${id}`);
    const data = await response.json();
    setDate(data.date);
    setLog(data.log);
    setLoading(false);
  }

  useEffect(() => {
    fetchLogData();
    const interval = setInterval(fetchLogData, 10000);
    return () => clearInterval(interval);
  }, []);

  return (
    <tr >
      {loading ? <td >Loading...</td> : <>
        <td >{date}</td>
        <td >{log}</td > </>}
    </tr >
  );
}

function Logs() {
  const count = 5;
  return (
    <div className="logs layout">
      <Table responsive="sm">
        <thead>
          <tr>
            <th>Time</th>
            <th>Log</th>
          </tr>
        </thead>
        <tbody>{[...Array(count).keys()].map(key => <LogItem key={key} id={key} />)}</tbody>
      </Table>
    </div>
  );
}

export default Logs;
