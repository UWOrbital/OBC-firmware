import Table from "react-bootstrap/Table";
import {
  QueryClient,
  QueryClientProvider,
  useQuery,
} from '@tanstack/react-query'

const queryClient = new QueryClient();

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

  const { data, isLoading } = useQuery({
    queryKey: ["aro-requests"],
    queryFn: async () => {
      const response = await fetch("http://localhost:5000/aro-request");
      if(!response.ok) throw new Error("Network response was not ok");
      return response.json();
    }
  });

  return (
    <QueryClientProvider client={queryClient}>
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
            {isLoading
              ? (
                <tr>
                  <td colSpan={4}>Loading...</td>
                </tr>
              )
              : (
                data.map((item: AROItemProps) => {
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
    </QueryClientProvider>
  );
}

export default ARORequests;
