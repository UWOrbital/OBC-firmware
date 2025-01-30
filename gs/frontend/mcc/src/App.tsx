import { BrowserRouter, Route, Routes } from "react-router-dom";
import Header from "./common/Header.tsx";
import Dashboard from "./dashboard/Dashboard.tsx";
import MissionCommands from "./mission_commands/MissionCommands.tsx";
import TelemetryData from "./telemetry/TelemetryData.tsx";
import ARORequests from "./aro_requests/ARORequests.tsx";
import Logs from "./common/Logs.tsx";
import NotFound from "./common/NotFound.tsx";

function App() {
  return (
    <BrowserRouter>
      <Header />
      <Routes>
        <Route path="*" element={<NotFound />} />
        <Route path="/" element={<Dashboard />} />
        <Route path="/mission-commands" element={<MissionCommands />} />
        <Route path="/telemetry-data" element={<TelemetryData />} />
        <Route path="/aro-requests" element={<ARORequests />} />
      </Routes>
      <Logs />
    </BrowserRouter>
  );
}

export default App;
