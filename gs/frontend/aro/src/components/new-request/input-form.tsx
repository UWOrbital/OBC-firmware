import React from "react";

interface InputFormProps {
  latitude: number | null;
  longitude: number | null;
  handleSubmit: (e: React.FormEvent<HTMLFormElement>) => void;
  handleLatitudeChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
  handleLongitudeChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
}

const InputForm: React.FC<InputFormProps> = ({
  latitude,
  longitude,
  handleSubmit,
  handleLatitudeChange,
  handleLongitudeChange,
}) => {
  return (
<div className="flex flex-col items-center justify-start min-h-screen bg-white text-lg pt-24">
  <div className="w-full max-w-lg p-8 bg-white rounded-lg">
    <h1 className="text-3xl text-center -mt-11 mb-10">New Request Form</h1>

    <form onSubmit={handleSubmit} className="flex flex-col space-y-6 w-full">
      <div className="flex flex-col w-full">
        <label className="text-base font-medium text-gray-700 mb-2">Latitude</label>
        <input
          type="number"
          name="latitude"
          placeholder="Enter your coordinates"
          value={latitude ?? ""}
          onChange={handleLatitudeChange}
          required
          className="w-full border border-gray-300 rounded-lg px-4 py-3 text-lg focus:outline-none focus:ring-2 focus:ring-blue-400"
        />
      </div>

      <div className="flex flex-col w-full">
        <label className="text-base font-medium text-gray-700 mb-4">Longitude</label>
        <input
          type="number"
          name="longitude"
          placeholder="Enter your coordinates"
          value={longitude ?? ""}
          onChange={handleLongitudeChange}
          required
          className="w-full border border-gray-300 rounded-lg px-4 py-3 text-lg focus:outline-none focus:ring-2 focus:ring-blue-400 mb-5"
        />
      </div>

      <div className="flex flex-col w-full space-y-4 mt-4">
        <button
          type="button"
          className="w-full bg-blue-500 hover:bg-blue-600 text-white font-medium py-3 rounded-lg shadow transition text-lg"
        >
          Validate Coordinates
        </button>
        <button
          type="submit"
          className="w-full border border-gray-300 text-black font-medium py-3 rounded-lg shadow-sm hover:bg-gray-50 transition text-lg"
        >
          Submit Request
        </button>
      </div>
    </form>
  </div>
</div>

  );
};

export default InputForm;
