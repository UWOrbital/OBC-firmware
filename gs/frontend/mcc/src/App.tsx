import { Routes, Route } from "react-router-dom";
import Nav from "./components/Nav";
import Background from "./components/Background";
import Commands from "./pages/Command/Commands";
import Dashboard from "./pages/Dashboard";
import AROAdmin from "./pages/AROAdmin";
import LiveSession from "./pages/LiveSession";
import Login from "./pages/Login";
import { ThemeProvider } from "./contexts/ThemeContext";

/**
 * @brief App component displaying the main application
 * @return tsx element of App component
 */
function App() {
  return (
    <ThemeProvider>
      <Nav />
      <Background />
      <div className="pt-16">
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/commands" element={<Commands />} />
          <Route path="/telemetry-data" element={<AROAdmin />} />
          <Route path="/aro-requests" element={<LiveSession />} />
          <Route path="/login" element={<Login />} />
        </Routes>
      </div>
    </ThemeProvider>
  );
}

export default App;
