import { Route, Routes } from "react-router-dom";
import Nav from "./components/Nav";
import Background from "./components/Background";
import NewRequestForm from "./components/new-request/new-request-form.tsx";

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
        <Route path="/" element={<Home />} />
        <Route path="/commands" element={<Commands />} />
        <Route path="/new-request" element={<NewRequestForm />} />
        <Route path="/requests" element={<Requests />} />
        <Route path="/profile" element={<Profile />} />
        <Route path="/login" element={<Login />} /> */}
        {<Route path="/new-request" element={<NewRequestForm />} />}
      </Routes>
    </>
  );
}

export default App;
