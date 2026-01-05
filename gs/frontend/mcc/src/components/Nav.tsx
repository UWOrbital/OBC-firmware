import { Link } from "react-router-dom";
import orbital_logo from "../assets/orbital_logo.png";
import { NAVIGATION_LINKS } from "../utils/nav-links";

/**
 * @brief Nav component displaying the navigation bar
 * @return tsx element of Nav component
 */
function Nav() {
  // TODO: create user auth that checks if the user is logged in
  const isLoggedIn = false;

  return (
    <nav className="m-7">
      {/* Logo */}
      <div className="fixed left-8">
        <Link to="/" className="hover:opacity-80 transition-opacity">
          <img src={orbital_logo} alt="orbital logo" className="h-12 w-auto" />
        </Link>
      </div>

      {/* Navigation Links */}
      <div className="fixed left-1/2 transform -translate-x-1/2 mt-3 flex space-x-7">
        {NAVIGATION_LINKS.map((link) => (
          <Link key={link.url} to={link.url} className="hover:underline">
            {link.text}
          </Link>
        ))}
      </div>

      {/* Profile or Login page, depending on authentication state */}
      {isLoggedIn ? (
        <div className="fixed right-8 mt-2 border-1 border-white rounded-xl p-1 px-2 hover:bg-white hover:text-black">
          <Link to="/profile" className="">
            Profile
          </Link>
        </div>
      ) : (
        <div className="fixed right-8 mt-2 border-1 border-white rounded-xl p-1 px-2 hover:bg-white hover:text-black">
          <Link to="/login" className="">
            Login
          </Link>
        </div>
      )}
    </nav>
  );
}

export default Nav;
