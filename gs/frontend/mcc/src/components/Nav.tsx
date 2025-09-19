import { Link } from "react-router-dom";
import orbital_logo from "../assets/orbital_logo.png";

function Nav() {
  return (
    <nav className="text-white m-7">
      {/* Logo */}
      <div className="absolute left-8">
        <Link to="/" className="hover:opacity-80 transition-opacity">
          <img src={orbital_logo} alt="orbital logo" className="h-12 w-auto" />
        </Link>
      </div>

      {/* Navigation Links */}
      <div className="absolute left-1/2 transform -translate-x-1/2 mt-3 flex space-x-7">
        <Link to="/" className="hover:underline">
          Dashboard
        </Link>
        <Link to="/mission-commands" className="hover:underline">
          Commands
        </Link>
        <Link to="/telemetry-data" className="hover:underline">
          ARO Admin
        </Link>
        <Link to="/aro-requests" className="hover:underline">
          Live Sessions
        </Link>
      </div>

      {/* Login */}
      <div className="absolute right-8 mt-2 border-1 border-white rounded-xl p-1 px-2 hover:bg-white hover:text-black">
        <Link to="/login" className="">
          Login
        </Link>
      </div>
    </nav>
  );
}

export default Nav;
