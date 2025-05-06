export interface RequestItemData {
  id: number;
  latitude: number;
  longitude: number;
  status: string;
  created_on: Date;
  request_sent_to_obc_on: Date | null;
  pic_taken_on: Date | null;
  pic_transmitted_on: Date | null;
  cancellable_after: Date;
  // Add more properties as needed
}
