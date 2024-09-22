import "./App.css";
import { BrowserRouter, Route, Routes } from "react-router-dom";
import Home from "./Home";
import Navbar from "./Navbar";
import NotFound from "./NotFound";
import InputForm from "./InputForm";
import Requests from "./Requests";
import Profile from "./Profile";

function App() {
	return (
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
	);
}

export default App;
