/**
 * @brief Login component displaying the login page
 * @return tsx element of Login component
 */
function Login() {
  return (
    <div className="flex flex-col items-center min-h-screen overflow-y-auto px-4 py-8">
      <div className="w-full max-w-md bg-gray-900/50 backdrop-blur-sm rounded-lg p-8 border border-gray-700/50 my-8">
        {/* Rocket Icon */}
        <div className="flex justify-center mb-4">
          <div className="w-16 h-16 bg-blue-600 rounded-full flex items-center justify-center">
            <svg
              className="w-8 h-8 text-white"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
              xmlns="http://www.w3.org/2000/svg"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M5 3v4M3 5h4M6 17v4m-2-2h4m5-16l2.286 6.857L21 12l-5.714 2.143L13 21l-2.286-6.857L5 12l5.714-2.143L13 3z"
              />
            </svg>
          </div>
        </div>

        {/* Title */}
        <h1 className="text-white text-2xl font-bold mb-2 text-center">
          UW Orbital MCC
        </h1>
        <p className="text-gray-400 text-sm mb-6 text-center">
          Mission Control Center Access
        </p>

        <form className="space-y-4">
          {/* Email Address */}
          <div>
            <label htmlFor="email" className="text-white block mb-2 text-sm font-medium">
              Email Address
            </label>
            <div className="relative">
              <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none">
                <svg
                  className="w-5 h-5 text-gray-400"
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                  xmlns="http://www.w3.org/2000/svg"
                >
                  <path
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    strokeWidth={2}
                    d="M3 8l7.89 5.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"
                  />
                </svg>
              </div>
              <input
                type="email"
                id="email"
                className="w-full bg-transparent text-gray-300 pl-10 pr-4 py-2.5 rounded-lg border border-gray-600/50 focus:outline-none focus:border-gray-500"
                placeholder="astronaut@uworbital.com"
              />
            </div>
          </div>

          {/* Password */}
          <div>
            <label htmlFor="password" className="text-white block mb-2 text-sm font-medium">
              Password
            </label>
            <div className="relative">
              <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none">
                <svg
                  className="w-5 h-5 text-gray-400"
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                  xmlns="http://www.w3.org/2000/svg"
                >
                  <path
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    strokeWidth={2}
                    d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z"
                  />
                </svg>
              </div>
              <input
                type="password"
                id="password"
                className="w-full bg-transparent text-gray-300 pl-10 pr-4 py-2.5 rounded-lg border border-gray-600/50 focus:outline-none focus:border-gray-500"
                placeholder="Enter your password"
              />
            </div>
          </div>

          {/* Remember me and Forgot password */}
          <div className="flex items-center justify-between text-sm">
            <label className="flex items-center text-gray-400 cursor-pointer">
              <input
                type="checkbox"
                className="mr-2 w-4 h-4 bg-transparent border border-gray-600 rounded"
              />
              Remember me
            </label>
            <a href="#" className="text-blue-500 hover:text-blue-400">
              Forgot password?
            </a>
          </div>

          {/* Submit Button */}
          <button
            type="submit"
            className="w-full cursor-pointer bg-blue-600 text-white py-2.5 rounded-lg hover:bg-blue-700 transition-colors mt-6 font-medium"
          >
            Access Mission Control
          </button>
        </form>

        {/* Signup link */}
        <p className="text-center text-gray-400 text-sm mt-6">
          Don't have an account?{" "}
          <a href="/signup" className="text-blue-500 hover:text-blue-400">
            Sign up
          </a>
        </p>

        {/* Footer */}
        <p className="text-center text-gray-500 text-xs mt-8">
          SECURE CONNECTION - MCC v2.0
        </p>
      </div>
    </div>
  );
}

export default Login;
