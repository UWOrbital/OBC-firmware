/**
 * Navigation links for the MCC frontend
 */

export interface NavLink {
  text: string;
  url: string;
}

export const NAVIGATION_LINKS: NavLink[] = [
  {
    text: "Dashboard",
    url: "/",
  },
  {
    text: "Commands",
    url: "/mission-commands",
  },
  {
    text: "ARO Admin",
    url: "/telemetry-data",
  },
  {
    text: "Live Sessions",
    url: "/aro-requests",
  },
];
