import Table from "react-bootstrap/Table";
import { useEffect, useState } from "react";

type AROItemProps = {
  id: number;
  latitude: number;
  longitude: number;
  status: string;
};
function AROItem({ id, latitude, longitude, status }: AROItemProps) {
  return (
    <>
      <tr>
        <td>{id}</td>
        <td>{longitude}</td>
        <td>{latitude}</td>
        <td>{status}</td>
      </tr>
    </>
  );
}

function ARORequests() {
  const [loading, setLoading] = useState(true);
  const [aroRequests, setARORequests] = useState<AROItemProps[]>([]);

  useEffect(() => {
    fetch("http://localhost:5000/aro-request")
      .then((response) => response.json())
      .then((data) => {
        setARORequests(data);
        setLoading(false);
      });
  }, []);

  return (
    <div className="arorequests layout">
      <Table striped bordered hover variant="light">
        <thead>
          <tr>
            <th>#</th>
            <th>Latitude</th>
            <th>Longitude</th>
            <th>Status</th>
          </tr>
        </thead>
        <tbody>
          {loading
            ? (
              <tr>
                <td colSpan={4}>Loading...</td>
              </tr>
            )
            : (
              aroRequests.map((item) => {
                return (
                  <AROItem
                    key={item.id}
                    id={item.id}
                    longitude={item.longitude}
                    latitude={item.latitude}
                    status={item.status}
                  />
                );
              })
            )}
        </tbody>
      </Table>
    </div>
  );
}

export default ARORequests;
