import "./new_request_form.css";
import { type ChangeEvent, useCallback, useEffect, useState } from "react";
import {
  APIProvider,
  Map,
  MapCameraChangedEvent,
} from "@vis.gl/react-google-maps";

const DEFAULT_ZOOM = 15;

const NewRequestForm = () => {
  const [latitude, setLatitude] = useState(0);
  const [longitude, setLongitude] = useState(0);
  const [mapLatitude, setMapLatitude] = useState(0);
  const [mapLongitude, setMapLongitude] = useState(0);
  const [zoom, setZoom] = useState(DEFAULT_ZOOM);

  useEffect(() => {
    navigator.geolocation.getCurrentPosition((position) => {
      setLatitude(position.coords.latitude);
      setLongitude(position.coords.longitude);
      setMapLatitude(position.coords.latitude);
      setMapLongitude(position.coords.longitude);
      // TODO: Show a map centered at latitude / longitude.
    });
  }, []);

  const handleSubmit = (event: React.FormEvent) => {
    event.preventDefault();
    updateMap();

    // TODO: Use the proper type for this
    const submission = {
      latitude,
      longitude,
    };
    // TODO: Submit form to backend
    console.log(submission);

    alert("Thanks for submitting!");
  };

  const updateMap = () => {
    setMapLatitude(latitude);
    setMapLongitude(longitude);
    setZoom(DEFAULT_ZOOM);
  };

  const handleLatitudeChange = (event: ChangeEvent<HTMLInputElement>) => {
    const value = parseFloat(event.target.value);
    setLatitude(value);
  };

  const handleLongitudeChange = (event: ChangeEvent<HTMLInputElement>) => {
    const value = parseFloat(event.target.value);
    setLongitude(value);
  };

  const handleCameraChange = useCallback((ev: MapCameraChangedEvent) => {
    setMapLatitude(ev.detail.center.lat);
    setMapLongitude(ev.detail.center.lng);
    setZoom(ev.detail.zoom);
  });

  // TODO: Add better error handling and switch to using react-hook-form
  return (
    <div className="new-request-page">
      <form className="input-form" onSubmit={handleSubmit} id="main-form">
        <label htmlFor="latitude">Latitude:</label>
        <input
          id="latitude"
          required
          type="number"
          placeholder="Enter your coordinates"
          value={latitude}
          onChange={handleLatitudeChange}
        />

        <label htmlFor="longitude">Longitude:</label>
        <input
          id="longitude"
          required
          type="number"
          placeholder="Enter your coordinates"
          value={longitude}
          onChange={handleLongitudeChange}
        />

        <input className="button" type="submit" />
        <input
          className="button"
          type="button"
          value="Update Map"
          onClick={updateMap}
        />
      </form>
      <div className="map">
        <APIProvider
          apiKey={import.meta.env.VITE_MAPS_API_KEY}
          onLoad={() => console.log("Maps API has loaded.")}
        >
          <Map
            zoom={zoom}
            center={{ lat: mapLatitude, lng: mapLongitude }}
            onCameraChanged={handleCameraChange}
          >
          </Map>
        </APIProvider>
      </div>
    </div>
  );
};

export default NewRequestForm;
