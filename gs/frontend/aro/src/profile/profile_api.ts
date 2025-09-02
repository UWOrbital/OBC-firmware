import { ProfileData } from "./profile_data.ts";

/**
 * @brief Gets the profile info of the current user
 */
export const getProfile = async (): Promise<ProfileData> => {
  // This is a mock implementation of an API call
  return {
    name: "John Doe",
    email: "john.doe@gmail.com",
    call_sign: "VAYPO",
    phone: "1234567890",
  };
};

/**
 * @brief Updates the profile info of the current user
 * @param data The new profile data
 */
export const updateProfile = async (data: ProfileData): Promise<void> => {
  // This is a mock implementation of an API call
  console.log(data);
};
