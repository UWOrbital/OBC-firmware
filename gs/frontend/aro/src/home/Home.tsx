import { Link } from "react-router-dom";
import "./Home.css";
import HomeStatus from "./HomeStatus.tsx";

function Home() {
	return (
		<main className="home">
			<h1>Welcome to the ARO Website by UWOrbital</h1>
			<HomeStatus />
			<div className="selection-buttons">
				{/*TODO: Switch to using buttons and have animation or something nice*/}
				<Link to={"new-request"} className="create-button">
					Create New Request
				</Link>
				<Link to={"requests"} className="view-button">
					View All Requests
				</Link>
			</div>
		</main>
	);
}

export default Home;
