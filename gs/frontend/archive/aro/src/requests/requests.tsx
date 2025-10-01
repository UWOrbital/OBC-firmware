import { type MouseEvent} from "react";
import type { RequestItemData } from "./request-item-data.ts";
import RequestItem from "./request-item.tsx";
import { getRequestItems } from "./requests-api.ts";
import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";

const Requests = () => {

  const {
    data = [],
    isLoading,
  } = useQuery<RequestItemData[]>({
    queryKey: ["requests"],
    queryFn: getRequestItems,
  });

  const queryClient = useQueryClient();

  // Removes the request with the given id from the list of data
  // TODO: Cancel request on the backend
  const cancelRequestMutation = useMutation({
    mutationFn: async (id: number) => {
      return id;
    },
    onSuccess: (id: number) => {
      queryClient.setQueryData<RequestItemData[]>(["requests"], (old) =>
        old ? old.filter((item) => item.id !== id) : []
      );
    },
  });

  const cancelRequest = (id: number) => {
    return async (a: MouseEvent) => {
      a.preventDefault();
      await cancelRequestMutation.mutateAsync(id);
    };
  };

  if (isLoading) {
    return <div>Loading requests...</div>;
  }

  if (!data || data.length === 0) {
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
        {data.map((item: RequestItemData) => {
          console.log(item);
          return (
          <tr key={item.id}>
            <RequestItem {...item} cancelRequest={cancelRequest(item.id)} />
          </tr>
          );
        })}
      </tbody>
    </table>
  );
};

export default Requests;
