import logo from "./assets/orbital-logo.png";
import { Link } from "react-router-dom";

function Navbar() {
	return (
		<div>
			<div className="nav">
				<img src={logo} alt="orbital logo"></img>
				<div className="navmenu">
					<Link to="/">Home</Link>
					<Link to="/about">About</Link>
					<Link to="/donate">Donate</Link>
				</div>
			</div>
		</div>
	);
}

export default Navbar;
