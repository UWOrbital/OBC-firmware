import Nav from "react-bootstrap/Nav";
import { Link } from "react-router-dom";

function Header() {
	return (
		<Nav className="header">
			<Nav.Item className="item">
				<Nav.Link eventKey="link-1">
					<Link to="/">Dashboard</Link>
				</Nav.Link>
			</Nav.Item>
			<Nav.Item className="item">
				<Nav.Link eventKey="link-2">
					<Link to="/mission-commands">Mission Commands</Link>
				</Nav.Link>
			</Nav.Item>
			<Nav.Item className="item">
				<Nav.Link eventKey="link-3">
					<Link to="/telemetry-data">Telemetry Data</Link>
				</Nav.Link>
			</Nav.Item>
			<Nav.Item className="item">
				<Nav.Link eventKey="link-4">
					<Link to="/aro-requests">ARO Requests</Link>
				</Nav.Link>
			</Nav.Item>
		</Nav>
	);
}

export default Header;
