import type { ProfileData } from "./profile-data.ts";

/**
 * @brief Gets the profile info of the current user
 */
export const getProfile = async (): Promise<ProfileData> => {
  try {
    const response = await fetch('http://localhost:5000/user/profile', {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
        // Add authentication headers here when implemented
        // 'Authorization': `Bearer ${token}`
      },
    });
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const data = await response.json();
    return data;
  } catch (error) {
    console.error('Error fetching profile:', error);
    throw error;
  }
};

/**
 * @brief Updates the profile info of the current user
 * @param data The new profile data
 */
export const updateProfile = async (data: ProfileData): Promise<void> => {
  try {
    const response = await fetch('http://localhost:5000/user/profile', {
      method: 'PUT',
      headers: {
        'Content-Type': 'application/json',
        // Add authentication headers here when implemented
        // 'Authorization': `Bearer ${token}`
      },
      body: JSON.stringify(data),
    });
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
  } catch (error) {
    console.error('Error updating profile:', error);
    throw error;
  }
};
