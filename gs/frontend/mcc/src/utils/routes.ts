/**
 * Routes for the MCC frontend
 */

export interface Route {
  text: string;
  url: string;
}

export const ROUTES: Route[] = [
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
