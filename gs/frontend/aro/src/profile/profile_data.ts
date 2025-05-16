export interface ProfileDataResponse {
  data: {
    first_name: string;
    last_name: string | undefined;
    email: string;
    phone_number: string;
    call_sign: string;
    id: string;
  };
  editable_fields: string[];
}
