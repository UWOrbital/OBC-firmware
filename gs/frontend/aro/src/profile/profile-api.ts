import { ProfileData } from "./profile-data";

/**
 * @brief Gets the profile info of the current user
 */
export const getProfile = async (): Promise<ProfileData> => {
	// This is a mock implementation of an API call
	return {
		name: "John Doe",
		email: "john.doe@gmail.com",
		call_sign: "VAYPO",
		phone: "123-456-7890",
	};
};
