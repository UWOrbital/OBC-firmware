import { RequestItemData } from "./request_item_data";

export const getRequestItems = async (): Promise<RequestItemData[]> => {
  return [
    {
      id: 1,
      status: "Pending",
      longitude: 100,
      latitude: 80,
      created_on: new Date(2024, 10),
      cancellable_after: new Date(2025, 12),
      request_sent_to_obc_on: null,
      pic_transmitted_on: null,
      pic_taken_on: null,
    },
    {
      id: 2,
      status: "Pending",
      longitude: 120,
      latitude: 80,
      created_on: new Date(2024, 10),
      cancellable_after: new Date(2025, 12),
      request_sent_to_obc_on: null,
      pic_transmitted_on: null,
      pic_taken_on: null,
    },
  ];
};
