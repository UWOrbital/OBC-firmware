import { type MouseEvent, useEffect, useState } from "react";
import RequestItem from "./request_item.tsx";
import type { RequestItemData } from "./request_item_data.ts";
import { getRequestItems } from "./requests_api.ts";

const Requests = () => {
  // TODO: Switch to using react-query
  const [data, setData] = useState<RequestItemData[]>([]);
  useEffect(() => {
    const getRequestItemsRegular = async () => {
      const response = await getRequestItems();
      setData(response);
    };
    getRequestItemsRegular();
  }, []);

  // Removes the request with the given id from the list of data
  // TODO: Cancel request on the backend
  const cancelRequest = (id: number) => {
    return async (_: MouseEvent) => {
      setData((prev: RequestItemData[]) =>
        prev.filter((item) => item.id != id)
      );
    };
  };

  if (data.length === 0) {
    return <div>You do not have any request created.</div>;
  }

  return (
    <table>
      <thead>
        <tr>
          <td>ID</td>
          <td>Latitude</td>
          <td>Longitude</td>
          <td>Status</td>
          <td>Created On</td>
          <td>Request Sent to OBC On</td>
          <td>Picture Taken On</td>
          <td>Picture transmitted On</td>
          <td>Download Packet</td>
          <td>Cancel Request</td>
        </tr>
      </thead>
      <tbody>
        {data.map((item: RequestItemData, key: number) => {
          console.log(item);
          return (
            <tr key={key}>
              {" "}
              <RequestItem {...item} cancelRequest={cancelRequest(item.id)} />
            </tr>
          );
        })}
      </tbody>
    </table>
  );
};

export default Requests;
