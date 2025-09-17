import "./new-request-form.css";
import { type ChangeEvent, useState } from "react";
import { MapContainer, TileLayer, useMap, Marker, Popup } from 'react-leaflet'
import 'leaflet/dist/leaflet.css';

const NewRequestForm = () => {
  const [latitude, setLatitude] = useState(0);
  const [longitude, setLongitude] = useState(0);

  navigator.geolocation.getCurrentPosition((position) => {
    setLatitude(position.coords.latitude);
    setLongitude(position.coords.longitude);
    // TODO: Show a map centered at latitude / longitude.
  });

  const handleSubmit = () => {
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
  const position: [number, number] = [51.505, -0.09];

  // TODO: Add better error handling and switch to using react-hook-form
  return (
    <div className = "form-container">
    <form className="input-form" onSubmit={handleSubmit} id="main-form">
      <label>Latitude</label>
      <input
        required
        type="number"
        placeholder="Enter your coordinates"
        value={latitude}
        onChange={handleLatitudeChange}
      />
      <label>Longitude</label>
      <input
        required
        type="number"
        placeholder="Enter your coordinates"
        value={longitude}
        onChange={handleLongitudeChange}
      />
      <input type="submit" />
    </form>

  <MapContainer
    center={[51.505, -0.09]}
    zoom={13}
    scrollWheelZoom={false}
    style={{ height: "50vh", width: "100%" }}
  >
    <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
      />
      <Marker position={position}>
        <Popup>A pretty CSS3 popup.</Popup>
      </Marker>
    </MapContainer>
    </div>
    );
};

export default NewRequestForm;
