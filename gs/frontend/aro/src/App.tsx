import "./App.css";
import { BrowserRouter, Route, Routes } from "react-router-dom";
import Home from "./Home";
import About from "./About";
import Donate from "./Donate";
import Navbar from "./Navbar";
import NotFound from "./NotFound";

function App() {
	return (
		<BrowserRouter>
			<Navbar />
			<Routes>
				<Route path="*" element={<NotFound />} />
				<Route path="/" element={<Home />} />
				<Route path="about" element={<About />} />
				<Route path="donate" element={<Donate />} />
			</Routes>
		</BrowserRouter>
	);
}

export default App;
