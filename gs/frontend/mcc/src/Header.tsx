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
          <Link to="missioncommands">Mission Commands</Link>
        </Nav.Link>
      </Nav.Item>
      <Nav.Item className="item">
        <Nav.Link eventKey="link-3">
          <Link to="telemetrydata">Telemetry Data</Link>
        </Nav.Link>
      </Nav.Item>
      <Nav.Item className="item">
        <Nav.Link eventKey="link-4">
          <Link to="arorequests">ARO Requests</Link>
        </Nav.Link>
      </Nav.Item>
    </Nav>
  );
}

export default Header;
