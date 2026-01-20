import { useState } from "react";
import FormInput from "../components/auth/FormInput";
import {
  EmailIcon,
  LockIcon,
  UserIcon,
  BuildingIcon,
  IDCardIcon,
  LocationIcon,
  RocketIcon,
} from "../components/auth/AuthIcons";

/**
 * Configuration for form fields - modify this to add/remove/change fields easily
 */
const FORM_CONFIG = {
  // Basic fields required for all users
  basicFields: [
    {
      id: "email",
      label: "Email Address",
      type: "email",
      placeholder: "astronaut@uworbital.com",
      icon: <EmailIcon />,
    },
    {
      id: "password",
      label: "Password",
      type: "password",
      placeholder: "Create a secure password",
      icon: <LockIcon />,
    },
    {
      id: "confirmPassword",
      label: "Confirm Password",
      type: "password",
      placeholder: "Re-enter your password",
      icon: <LockIcon />,
    },
  ],
  // Additional fields for operators
  operatorFields: [
    {
      id: "firstName",
      label: "First Name",
      type: "text",
      placeholder: "John",
      icon: <UserIcon />,
    },
    {
      id: "lastName",
      label: "Last Name",
      type: "text",
      placeholder: "Doe",
      icon: <UserIcon />,
    },
    {
      id: "institution",
      label: "Institution",
      type: "text",
      placeholder: "University of Waterloo",
      icon: <BuildingIcon />,
    },
    {
      id: "identification",
      label: "Identification Number",
      type: "text",
      placeholder: "ID-12345678",
      icon: <IDCardIcon />,
    },
    {
      id: "address",
      label: "Address",
      type: "text",
      placeholder: "123 Orbital Drive, Waterloo, ON",
      icon: <LocationIcon />,
    },
  ],
};

/**
 * @brief Signup component for user registration
 * @return tsx element of Signup component
 */
function Signup() {
  const [isOperator, setIsOperator] = useState(false);
  const [formData, setFormData] = useState({
    email: "",
    password: "",
    confirmPassword: "",
    firstName: "",
    lastName: "",
    institution: "",
    identification: "",
    address: "",
  });

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]: value,
    }));
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    // Validate passwords match
    if (formData.password !== formData.confirmPassword) {
      alert("Passwords do not match!");
      return;
    }

    // TODO: Implement actual signup logic here
    console.log("Signup data:", {
      ...formData,
      role: isOperator ? "operator" : "user",
    });

    alert("Signup successful! (This is a placeholder)");
  };

  return (
    <div className="flex flex-col items-center h-screen overflow-y-auto px-4 py-8">
      <div className="w-full max-w-2xl bg-gray-900/50 backdrop-blur-sm rounded-lg p-8 border border-gray-700/50 my-auto">
        {/* Rocket Icon */}
        <div className="flex justify-center mb-4">
          <div className="w-16 h-16 bg-blue-600 rounded-full flex items-center justify-center">
            <RocketIcon />
          </div>
        </div>

        {/* Title */}
        <h1 className="text-white text-2xl font-bold mb-2 text-center">
          Join UW Orbital MCC
        </h1>
        <p className="text-gray-400 text-sm mb-6 text-center">
          Create your Mission Control Center account
        </p>

        <form onSubmit={handleSubmit} className="space-y-4">
          {/* Role Selection */}
          <div className="bg-gray-800/30 p-4 rounded-lg border border-gray-700/50">
            <label className="text-white block mb-3 text-sm font-medium">
              Account Type <span className="text-red-400">*</span>
            </label>
            <div className="flex gap-4">
              <button
                type="button"
                onClick={() => setIsOperator(false)}
                className={`flex-1 py-3 px-4 rounded-lg border transition-all ${
                  !isOperator
                    ? "bg-blue-600 border-blue-500 text-white"
                    : "bg-transparent border-gray-600 text-gray-400 hover:border-gray-500"
                }`}
              >
                <div className="font-medium">Standard User</div>
                <div className="text-xs mt-1 opacity-80">Basic access</div>
              </button>
              <button
                type="button"
                onClick={() => setIsOperator(true)}
                className={`flex-1 py-3 px-4 rounded-lg border transition-all ${
                  isOperator
                    ? "bg-blue-600 border-blue-500 text-white"
                    : "bg-transparent border-gray-600 text-gray-400 hover:border-gray-500"
                }`}
              >
                <div className="font-medium">Operator</div>
                <div className="text-xs mt-1 opacity-80">Full access</div>
              </button>
            </div>
          </div>

          {/* Basic Fields - Always shown */}
          <div className="space-y-4">
            {FORM_CONFIG.basicFields.map((field) => (
              <FormInput
                key={field.id}
                id={field.id}
                label={field.label}
                type={field.type}
                placeholder={field.placeholder}
                icon={field.icon}
                required
                value={formData[field.id as keyof typeof formData]}
                onChange={handleInputChange}
              />
            ))}
          </div>

          {/* Operator Additional Fields - Conditionally shown */}
          {isOperator && (
            <div className="space-y-4 pt-4 border-t border-gray-700/50">
              <p className="text-gray-400 text-sm mb-2">
                Additional information required for operator access
              </p>
              {FORM_CONFIG.operatorFields.map((field) => (
                <FormInput
                  key={field.id}
                  id={field.id}
                  label={field.label}
                  type={field.type}
                  placeholder={field.placeholder}
                  icon={field.icon}
                  required
                  value={formData[field.id as keyof typeof formData]}
                  onChange={handleInputChange}
                />
              ))}
            </div>
          )}

          {/* Terms and Conditions */}
          <div className="flex items-start text-sm pt-2">
            <input
              type="checkbox"
              id="terms"
              required
              className="mr-2 mt-1 w-4 h-4 bg-transparent border border-gray-600 rounded"
            />
            <label htmlFor="terms" className="text-gray-400">
              I agree to the{" "}
              <a href="#" className="text-blue-500 hover:text-blue-400">
                Terms of Service
              </a>{" "}
              and{" "}
              <a href="#" className="text-blue-500 hover:text-blue-400">
                Privacy Policy
              </a>
            </label>
          </div>

          {/* Submit Button */}
          <button
            type="submit"
            className="w-full bg-blue-600 text-white py-2.5 rounded-lg hover:bg-blue-700 transition-colors mt-6 font-medium"
          >
            Create Account
          </button>
        </form>

        {/* Login link */}
        <p className="text-center text-gray-400 text-sm mt-6">
          Already have an account?{" "}
          <a href="/login" className="text-blue-500 hover:text-blue-400">
            Sign in
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

export default Signup;
