import { Routes, Route } from "react-router-dom";
import Nav from "./components/Nav";
import Background from "./components/Background";
import Commands from "./pages/Command/Commands";
import Dashboard from "./pages/Dashboard";
import AROAdmin from "./pages/AROAdmin";
import LiveSession from "./pages/LiveSession";
import Login from "./pages/Login";

import { ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';

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
        <Route path="/commands" element={<Commands />} />
        <Route path="/telemetry-data" element={<AROAdmin />} />
        <Route path="/aro-requests" element={<LiveSession />} />
        <Route path="/login" element={<Login />} />
      </Routes>
      <ToastContainer />
    </>
  );
}

export default App;
