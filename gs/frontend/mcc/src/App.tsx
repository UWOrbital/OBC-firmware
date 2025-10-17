import { Routes, Route } from "react-router-dom";
import Nav from "./components/Nav";
import Background from "./components/Background";
import Dashboard from "./pages/Dashboard";
import Commands from "./pages/Commands";
import AROAdmin from "./pages/AROAdmin";
import LiveSession from "./pages/LiveSession";
import Login from "./pages/Login";

/**
 * @brief App component displaying the main application
 * @return tsx element of App component
 */
function App() {
  return (
    <>
      <Nav />
      <Background />
      <Routes>
        <Route path="/" element={<Dashboard />} />
        <Route path="/mission-commands" element={<Commands />} />
        <Route path="/telemetry-data" element={<AROAdmin />} />
        <Route path="/aro-requests" element={<LiveSession />} />
        <Route path="/login" element={<Login />} />
      </Routes>
    </>
  );
}

export default App;
