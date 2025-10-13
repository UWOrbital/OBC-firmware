export interface Command {
  id: number;
  name: string;
  params: string;
  format: string;
  data_size: number;
  total_size: number;
}