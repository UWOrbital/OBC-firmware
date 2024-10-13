import { BrowserRouter, Routes, Route } from "react-router-dom";
import Header from "./common/Header";
import Dashboard from "./dashboard/Dashboard";
import MissionCommands from "./mission_commands/MissionCommands";
import TelemetryData from "./telemetry/TelemetryData";
import ARORequests from "./aro_requests/ARORequests";
import Logs from "./common/Logs";
import NotFound from "./common/NotFound";

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
