import { ReactNode } from "react";

interface FormInputProps {
  id: string;
  label: string;
  type?: string;
  placeholder: string;
  icon?: ReactNode;
  required?: boolean;
  value?: string;
  onChange?: (e: React.ChangeEvent<HTMLInputElement>) => void;
}

/**
 * @brief Reusable form input component with label and optional icon
 * @param props FormInputProps
 * @return tsx element of FormInput component
 */
function FormInput({
  id,
  label,
  type = "text",
  placeholder,
  icon,
  required = false,
  value,
  onChange,
}: FormInputProps) {
  return (
    <div>
      <label htmlFor={id} className="text-white block mb-2 text-sm font-medium">
        {label} {required && <span className="text-red-400">*</span>}
      </label>
      <div className="relative">
        {icon && (
          <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none">
            {icon}
          </div>
        )}
        <input
          type={type}
          id={id}
          name={id}
          required={required}
          value={value}
          onChange={onChange}
          className={`w-full bg-transparent text-gray-300 ${
            icon ? "pl-10" : "pl-4"
          } pr-4 py-2.5 rounded-lg border border-gray-600/50 focus:outline-none focus:border-gray-500 focus:ring-1 focus:ring-gray-500`}
          placeholder={placeholder}
        />
      </div>
    </div>
  );
}

export default FormInput;
