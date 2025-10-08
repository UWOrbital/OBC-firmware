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

      if (isInvalidCoordinate(newLatitude, newLongitude)) alert("Please enter valid coordinates!");
      else {
        setLatitude(newLatitude);
        setLongitude(newLongitude);
        if (action === "Submit") {
          // TODO: Submit coordiantes to backend
          alert(`Request submitted at (${newLatitude}, ${newLongitude})`);
        }
      }
    }
  };

  const handleCoordinateChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    const name = e.target.name;
    if (name == "latitude") {
        if (value === "") {
          setLatitude(null);
        } else {
          const num = parseFloat(value);
          if (!isNaN(num)) setLatitude(num);
        }
    } else {
        if (value === "") {
          setLongitude(null);
        } else {
          const num = parseFloat(value);
          if (!isNaN(num)) setLongitude(num);
        }
    }
  };

  function isInvalidCoordinate(lat: number, lng: number) {
    if (lat < -90 || lat > 90 || lng < -180 || lng > 180) {
      return true;
    } else {
      return false;
    }
  }

  return (
    <div className = "form-container flex mt-25">
      <div className="w-1/4 max-h-500 overflow-auto">
        <InputForm
          latitude={latitude}
          longitude={longitude}
          handleSubmit={handleSubmit}
          handleLatitudeChange={handleCoordinateChange}
          handleLongitudeChange={handleCoordinateChange}
        />
      </div>
  {latitude !== null && longitude !== null && (
    <div className="flex-1">
      <MapView
        latitude={latitude}
        longitude={longitude}
        setLatitude={setLatitude}
        setLongitude={setLongitude}
      />
    </div>
  )}
  </div>  );
};
export default NewRequestForm;
