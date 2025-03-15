import "./new_request_form.css";
import { type ChangeEvent, useState } from "react";

const NewRequestForm = () => {
  const [latitude, setLatitude] = useState(0);
  const [longitude, setLongitude] = useState(0);

  navigator.geolocation.getCurrentPosition((position) => {
    setLatitude(position.coords.latitude);
    setLongitude(position.coords.longitude);
    // TODO: Show a map centered at latitude / longitude.
  });

  const handleSubmit = (_: unknown) => {
    // TODO: Use the proper type for this
    const submission = {
      latitude,
      longitude,
    };
    // TODO: Submit form to backend
    console.log(submission);

    alert("Thanks for submitting!");
  };

  const handleLatitudeChange = (event: ChangeEvent<HTMLInputElement>) => {
    const value = parseFloat(event.target.value);
    setLatitude(value);
  };

  const handleLongitudeChange = (event: ChangeEvent<HTMLInputElement>) => {
    const value = parseFloat(event.target.value);
    setLongitude(value);
  };

  // TODO: Add better error handling and switch to using react-hook-form
  return (
    <form className="input-form" onSubmit={handleSubmit} id="main-form">
      <div>
        <label>Latitude:</label>
        <input
          required
          type="number"
          placeholder="Enter your coordinates"
          value={latitude}
          onChange={handleLatitudeChange}
        />
      </div>
      <div>
        <label>Longitude:</label>
        <input
          required
          type="number"
          placeholder="Enter your coordinates"
          value={longitude}
          onChange={handleLongitudeChange}
        />
      </div>
      <div>
        <input className="button" type="submit" />
        <input className="button" type="submit" value="Update Map" />
      </div>
    </form>
  );
};

export default NewRequestForm;
