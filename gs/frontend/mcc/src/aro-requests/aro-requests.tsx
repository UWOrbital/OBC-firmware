import Table from "react-bootstrap/Table";
import { useQuery } from '@tanstack/react-query';

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
  const { data, isLoading} = useQuery<AROItemProps[]>({
  queryKey: ["aro-requests"],
  queryFn: async () => {
    const response = await fetch("http://localhost:8000/api/v1/mcc/requests/");
    if(!response.ok) throw new Error("Network response was not ok");
    return response.json();
  },
  });

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
          {isLoading || !data
            ? (
              <tr>
                <td colSpan={4}>Loading...</td>
              </tr>
            )
            :(
              data.map((item) => {
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
