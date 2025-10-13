import { Routes, Route } from "react-router-dom";
import Nav from "./components/Nav";
import Background from "./components/Background";
import Commands from "./pages/Command/CommandsPage";

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
        {/* uncomment these routes as the pages are created */}
        {/* <Route path="*" element={<Dashboard />} />
        <Route path="/" element={<Dashboard />} /> */}
        <Route path="/commands" element={<Commands />} />
        {/* <Route path="/aro-admin" element={<AROAdmin />} />
        <Route path="/live-sessions" element={<LiveSessions />} />
        <Route path="/login" element={<Login />} /> */}
      </Routes>
    </>
  );
}

export default App;
