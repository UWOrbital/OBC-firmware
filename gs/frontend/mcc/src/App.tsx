import Nav from "./components/Nav";
import { Routes } from "react-router-dom";
import Background from "./components/Background";

import { ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import './styles/toasts.css';

/**
 * @brief App component displaying the main application
 * @return tsx element of App component
 */
function App() {
return (
    <>
    <Nav/>
    <Routes>
        {/* uncomment these routes as the pages are created */}
        {/* <Route path="*" element={<Dashboard />} />
        <Route path="/" element={<Dashboard />} />
        <Route path="/commands" element={<Commands />} />
        <Route path="/aro-admin" element={<AROAdmin />} />
        <Route path="/live-sessions" element={<LiveSessions />} />
        <Route path="/login" element={<Login />} /> */}
    </Routes>
    <Background/>
    <ToastContainer />
    </>
);
}

export default App;
