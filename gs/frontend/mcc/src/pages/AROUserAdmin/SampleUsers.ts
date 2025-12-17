import type { User } from "./Columns";

export const SampleUsers: User[] = [
  { id: "1", username: "hyperion_admin", fullName: "Lucas Brennan", email: "lucas.brennan@example.com", dateCreated: "Jan 02, 2025", callsign: "VE3LBN", role: "Admin" },
  { id: "2", username: "midnightwave", fullName: "Ava Mitchell", email: "ava.mitchell@example.com", dateCreated: "Jan 03, 2025", callsign: "VA3AMT", role: "User" },

  { id: "3", username: "quantummaster", fullName: "Dylan Hughes", email: "dylan.hughes@example.com", dateCreated: "Jan 04, 2025", callsign: "KJ6DLH", role: "Admin" },
  { id: "4", username: "silverpetal", fullName: "Isla Park", email: "isla.park@example.com", dateCreated: "Jan 05, 2025", callsign: "VE6IPK", role: "User" },

  { id: "5", username: "radiocontrol", fullName: "Ethan Wallace", email: "ethan.wallace@example.com", dateCreated: "Jan 06, 2025", callsign: "K7EWC", role: "Admin" },
  { id: "6", username: "softmirage", fullName: "Nora Diaz", email: "nora.diaz@example.com", dateCreated: "Jan 07, 2025", callsign: "VA2NDZ", role: "User" },

  { id: "7", username: "techwarden", fullName: "Gavin Stewart", email: "gavin.stewart@example.com", dateCreated: "Jan 08, 2025", callsign: "VE4GVS", role: "Admin" },
  { id: "8", username: "pixelvine", fullName: "Lily Carter", email: "lily.carter@example.com", dateCreated: "Jan 09, 2025", callsign: "VE1LYC", role: "User" },

  { id: "9", username: "atlasnode", fullName: "Colin Marsh", email: "colin.marsh@example.com", dateCreated: "Jan 10, 2025", callsign: "K5CMS", role: "Admin" },
  { id: "10", username: "neonprism", fullName: "Sofia Bennett", email: "sofia.bennett@example.com", dateCreated: "Jan 11, 2025", callsign: "VE7SBN", role: "User" },

  { id: "11", username: "logicpulse", fullName: "Henry Dawson", email: "henry.dawson@example.com", dateCreated: "Jan 12, 2025", callsign: "KD9HDW", role: "Admin" },
  { id: "12", username: "auroraflake", fullName: "Chloe Patel", email: "chloe.patel@example.com", dateCreated: "Jan 13, 2025", callsign: "VA5CPT", role: "User" },

  { id: "13", username: "adminforge", fullName: "Owen Clarke", email: "owen.clarke@example.com", dateCreated: "Jan 14, 2025", callsign: "VE3OCN", role: "Admin" },
  { id: "14", username: "bloomcipher", fullName: "Emma Sullivan", email: "emma.sullivan@example.com", dateCreated: "Jan 15, 2025", callsign: "K6EMS", role: "User" },

  { id: "15", username: "bitarchitect", fullName: "Julian Brooks", email: "julian.brooks@example.com", dateCreated: "Jan 16, 2025", callsign: "KF4JLB", role: "Admin" },
  { id: "16", username: "nightflora", fullName: "Maya Chen", email: "maya.chen@example.com", dateCreated: "Jan 17, 2025", callsign: "VA3MYC", role: "User" },

  { id: "17", username: "ionmaster", fullName: "Sawyer Lee", email: "sawyer.lee@example.com", dateCreated: "Jan 18, 2025", callsign: "K9SWL", role: "Admin" },
  { id: "18", username: "bluespark", fullName: "Zara Collins", email: "zara.collins@example.com", dateCreated: "Jan 19, 2025", callsign: "VE2ZRC", role: "User" },

  { id: "19", username: "adminvector", fullName: "Miles Turner", email: "miles.turner@example.com", dateCreated: "Jan 20, 2025", callsign: "KD2MLT", role: "Admin" },
  { id: "20", username: "solsticebyte", fullName: "Bella Nguyen", email: "bella.nguyen@example.com", dateCreated: "Jan 21, 2025", callsign: "VA6BLN", role: "User" },

  ...Array.from({ length: 80 }).map((_, i) => {
    const id = i + 21;
    const isAdmin = id % 2 === 1;
    return {
      id: id.toString(),
      username: isAdmin
        ? `admin_${id}_node`
        : `user_${id}_spark`,
      fullName: isAdmin
        ? `Admin Person ${id}`
        : `User Person ${id}`,
      email: isAdmin
        ? `admin${id}@example.com`
        : `user${id}@example.com`,
      dateCreated: `Feb ${((id - 20) % 28) + 1}, 2025`,
      callsign: isAdmin
        ? `ADM${id}CS`
        : `USR${id}CS`,
      role: isAdmin ? "Admin" : "User",
    };
  }),
];
