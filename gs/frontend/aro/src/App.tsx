import "App.css";
import { BrowserRouter, Route, Routes } from "react-router-dom";
import Home from "home/Home";
import Navbar from "common/Navbar";
import NotFound from "common/NotFound";
import InputForm from "new-request/InputForm";
import Requests from "requests/Requests";
import Profile from "profile/Profile";
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
					<Route path="/new-request" element={<InputForm />} />
					<Route path="/requests" element={<Requests />} />
					<Route path="/profile" element={<Profile />} />
				</Routes>
			</BrowserRouter>
		</QueryClientProvider>
	);
}

export default App;
