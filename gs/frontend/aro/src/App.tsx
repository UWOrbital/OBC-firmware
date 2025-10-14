import { Routes } from "react-router-dom";
import Nav from "./components/Nav";
import Background from "./components/Background";

import { ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import './styles/toasts.css';

import toastService from './services/Toast.service'

/**
 * @brief App component displaying the main application
 * @return tsx element of App component
 */
function App() {
const showSuccess = () => {
    toastService.success("This is a success message for testing!");
};

const showError = () => {
    toastService.error("This is an error message for testing.");
};
  return (
    <>
      <Nav/>
      <Background/>
      <Routes>
        {/* uncomment these routes as the pages are created */}
        {/* <Route path="*" element={<Dashboard />} />
        <Route path="/" element={<Home />} />
        <Route path="/commands" element={<Commands />} />
        <Route path="/new-request" element={<NewRequestForm />} />
        <Route path="/requests" element={<Requests />} />
        <Route path="/profile" element={<Profile />} />
        <Route path="/login" element={<Login />} /> */}
      </Routes>
      {/* --- TEST BUTTONS --- */}
    <div className="w-1/4 flex flex-row mt-[10%] ms-[34.5%]">
        <h3>Toast Test Controls</h3>
        <button onClick={showSuccess} className="bg-lime-400 w-1/2 cursor-pointer">Success</button>
        <button onClick={showError} className="bg-red-400 btn w-1/2 cursor-pointer">Error</button>
    </div>
    {/* --- END --- */}
    <ToastContainer />
    </>
  );
}

export default App;
