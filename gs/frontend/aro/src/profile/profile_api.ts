import { ProfileDataResponse } from "./profile_data.ts";
import axios from "npm:axios";

const API_BASE = "http://localhost:8000/api/v1/";
const API_ARO_USER = `${API_BASE}aro/user/`;

/**
 * @brief Gets the profile info of the current user
 */
export const getProfile = async (): Promise<ProfileDataResponse> => {
  const response = await axios.get<ProfileDataResponse>(`${API_ARO_USER}`);
  return response.data;
};

/**
 * @brief Updates the profile info of the current user
 * @param data The new profile data
 */
export const updateProfile = async (
  data: ProfileDataResponse,
): Promise<ProfileDataResponse> => {
  // This is a mock implementation of an API call
  console.log(data);
  return {
    data: {
      first_name: "John",
      last_name: "Doe",
      email: "john.doe@gmail.com",
      call_sign: "VAYPO",
      phone_number: "1234567890",
      id: "123",
    },
    editable_fields: ["first_name", "last_name", "call_sign", "phone_number"],
  };
};
