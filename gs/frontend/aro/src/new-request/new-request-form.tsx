import "./new-request-form.css";
import { useState, useEffect} from "react";
import { MapContainer, TileLayer, useMap, Marker, Popup, useMapEvents } from 'react-leaflet'
import 'leaflet/dist/leaflet.css';

const NewRequestForm = () => {
  const [latitude, setLatitude] = useState<number | null>(null);
  const [longitude, setLongitude] = useState<number | null>(null);

  useEffect(() => {
    navigator.geolocation.getCurrentPosition(
      (position) => {
        setLatitude(position.coords.latitude);
        setLongitude(position.coords.longitude);
      }
    );
  }, []);

  const handleSubmit = (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    const formData = new FormData(e.currentTarget);
    const newLatitude = parseFloat(formData.get("latitude") as string);
    const newLongitude = parseFloat(formData.get("longitude") as string);
    
    const submitter = (e.nativeEvent as SubmitEvent).submitter as HTMLInputElement;
    const action = submitter?.value;
    
    if (newLatitude !== null && !isNaN(newLatitude) &&
      newLongitude !== null && !isNaN(newLongitude)) {
      
      if (newLatitude < - 90 || newLatitude > 90 || newLongitude < -180 || newLongitude > 180) alert("Please enter valid coordinates!");
      else {
        setLatitude(newLatitude);
        setLongitude(newLongitude);
        if (action === "Submit") {
          alert(`Request submitted at (${newLatitude}, ${newLongitude})`);
        }
      }
    }
  };

  const handleLatitudeChange = (e: React.ChangeEvent<HTMLInputElement>,
        setLatitude: React.Dispatch<React.SetStateAction<number | null>>
  ) => {
    const value = e.target.value;
    if (value === "") {
      setLatitude(null);
    } else {
      const num = parseFloat(value);
      if (!isNaN(num)) setLatitude(num);
    }
  };

  const handleLongitudeChange = (e: React.ChangeEvent<HTMLInputElement>,
        setLongitude: React.Dispatch<React.SetStateAction<number | null>>
  ) => {
    const value = e.target.value;
    if (value === "") {
      setLongitude(null);
    } else {
      const num = parseFloat(value);
      if (!isNaN(num)) setLongitude(num);
    }
  };

  function RecenterMap({ lat, lng }: { lat: number; lng: number }) {
    const map = useMap();
    map.setView([lat, lng]);
    return null;
  }

  function LocationSelector() {
  useMapEvents({
    click(e) {
      const event = e.originalEvent as MouseEvent;

      if (event.shiftKey) {
        setLatitude(e.latlng.lat);
        setLongitude(e.latlng.lng);
      }
    },
  });
  return null;
}

  return (
    <div className = "form-container">
    <form className="input-form" onSubmit={handleSubmit} id="main-form">
      <label>Latitude</label>
      <input
        required
        type="number"
        name="latitude"
        placeholder="Enter your coordinates"
        value={latitude ?? ""}
        onChange={(e) => handleLatitudeChange(e, setLatitude)}
      />
      <label>Longitude</label>
      <input
        required
        type="number"
        name="longitude"
        placeholder="Enter your coordinates"
        value={longitude ?? ""}
        onChange={(e) => handleLongitudeChange(e, setLongitude)}
      />
      <input type="submit" name="action" value="Check" className="submit-button"/>
      <input type="submit" name="action" value="Submit" className="submit-button"/>
    </form>

  {latitude !== null && longitude !== null && (
  <MapContainer
    center={[latitude, longitude]}
    zoom={7}
    scrollWheelZoom={false}
    style={{ height: "70vh", width: "100%" }}
    doubleClickZoom={false} 
    boxZoom={false} 
    keyboard={false}
  > 
    <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
    />
    <Marker position={[latitude, longitude]}>
      <Popup>The selected request position</Popup>
    </Marker>
    <LocationSelector/>
    <RecenterMap lat={latitude} lng={longitude} />
  </MapContainer> )}
  </div>  );
};
export default NewRequestForm;
