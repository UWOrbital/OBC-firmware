import "./App.css";
import { BrowserRouter, Route, Routes } from "react-router-dom";
import Home from "./home/Home.tsx";
import Navbar from "./common/Navbar.tsx";
import NotFound from "./common/NotFound.tsx";
import NewRequestForm from "./new-request/NewRequestForm.tsx";
import Requests from "./requests/Requests.tsx";
import Profile from "./profile/Profile.tsx";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";

const queryClient = new QueryClient();

function App() {
  return (
    <QueryClientProvider client={queryClient}>
      <BrowserRouter>
        <Navbar />
        <Routes>
          <Route path="*" element={<NotFound />} />
          <Route path="/" element={<Home />} />
          <Route path="/new-request" element={<NewRequestForm />} />
          <Route path="/requests" element={<Requests />} />
          <Route path="/profile" element={<Profile />} />
        </Routes>
      </BrowserRouter>
    </QueryClientProvider>
  );
}

export default App;
