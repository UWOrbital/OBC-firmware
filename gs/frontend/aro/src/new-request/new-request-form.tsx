import "./new-request-form.css";
import { type ChangeEvent, useState, useEffect} from "react";
import { MapContainer, TileLayer, useMap, Marker, Popup } from 'react-leaflet'
import 'leaflet/dist/leaflet.css';

const NewRequestForm = () => {
  const [latitude, setLatitude] = useState(0);
  const [longitude, setLongitude] = useState(0);

  useEffect(() => {
    navigator.geolocation.getCurrentPosition((position) => {
      setLatitude(position.coords.latitude);
      setLongitude(position.coords.longitude);
    });
  }, []);

  const handleSubmit = (e: React.FormEvent<HTMLFormElement>) => {
  e.preventDefault();
  const formData = new FormData(e.currentTarget); // currentTarget is typed correctly
  const newLatitude = parseFloat(formData.get("latitude") as string);
  const newLongitude = parseFloat(formData.get("longitude") as string);
  
  if (newLatitude !== null && !isNaN(newLatitude) &&
    newLongitude !== null && !isNaN(newLongitude)) {
    
    if (newLatitude < - 90 || newLatitude > 90 || newLongitude < -180 || newLongitude > 180) alert("Please enter valid coordinates!");
    else {
      setLatitude(newLatitude);
      setLongitude(newLongitude);
    }
  }
};

  function RecenterMap({ lat, lng }: { lat: number; lng: number }) {
    const map = useMap();
    map.setView([lat, lng]);
    return null;
  }

  // TODO: Add better error handling and switch to using react-hook-form
  return (
    <div className = "form-container">
    <form className="input-form" onSubmit={handleSubmit} id="main-form">
      <label>Latitude</label>
      <input
        required
        type="number"
        name="latitude"
        placeholder="Enter your coordinates"
        defaultValue={latitude}
      />
      <label>Longitude</label>
      <input
        required
        type="number"
        name="longitude"
        placeholder="Enter your coordinates"
        defaultValue={longitude}
      />
      <input type="submit" value="Check" className="submit-button"/>
    </form>

  <MapContainer
    center={[latitude, longitude]}
    zoom={7}
    scrollWheelZoom={false}
    style={{ height: "70vh", width: "100%" }}
  >
    <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
      />
      <Marker position={[latitude, longitude]}>
        <Popup>A pretty CSS3 popup.</Popup>
      </Marker>
      <RecenterMap lat={latitude} lng={longitude} />
    </MapContainer>
    </div>
    );
};

export default NewRequestForm;
