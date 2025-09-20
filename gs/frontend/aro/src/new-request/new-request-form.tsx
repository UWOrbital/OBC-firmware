import "./new-request-form.css";
import React, { useState, useEffect} from "react";
import InputForm from "./input-form";
import MapView from "./map-view";

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

  const handleLatitudeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === "") {
      setLatitude(null);
    } else {
      const num = parseFloat(value);
      if (!isNaN(num)) setLatitude(num);
    }
  };

  const handleLongitudeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === "") {
      setLongitude(null);
    } else {
      const num = parseFloat(value);
      if (!isNaN(num)) setLongitude(num);
    }
  };
  return (
    <div className = "form-container">
    <InputForm
      latitude={latitude}
      longitude={longitude}
      handleSubmit={handleSubmit}
      handleLatitudeChange={handleLatitudeChange}
      handleLongitudeChange={handleLongitudeChange}
    />
  {latitude !== null && longitude !== null && (
    <MapView
      latitude={latitude}
      longitude={longitude}
      setLatitude={setLatitude}
      setLongitude={setLongitude}
    />
  )}
  </div>  );
};
export default NewRequestForm;

