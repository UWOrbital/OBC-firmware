import { BrowserRouter, Routes, Route } from 'react-router-dom'
import Header from './Header'
import Dashboard from './Dashboard'
import MissionCommands from './MissionCommands'
import TelemetryData from './TelemetryData'
import ARORequests from './ARORequests'
import Logs from './Logs'

function App() {
  return (
    <BrowserRouter>
      <Header />
      <Routes>
        <Route path="/" element={<Dashboard />} />
        <Route path="missioncommands" element={<MissionCommands />} />
        <Route path="telemetrydata" element={<TelemetryData />} />
        <Route path="arorequests" element={<ARORequests />} />
      </Routes>
      <Logs />
    </BrowserRouter>
  )
}

export default App
