import { BrowserRouter, Routes } from "react-router-dom";

function App() {
  return (
    <BrowserRouter>
      {/* <Header /> */}
      <Routes>
        {/* <Route path="*" element={<NotFound />} />
        <Route path="/" element={<Dashboard />} />
        <Route path="/mission-commands" element={<MissionCommands />} />
        <Route path="/telemetry-data" element={<TelemetryData />} />
        <Route path="/aro-requests" element={<ARORequests />} /> */}
      </Routes>
      {/* <Logs /> */}
    </BrowserRouter>
  );
}

export default App;
