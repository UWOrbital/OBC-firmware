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
    <form className="input-form" onSubmit={handleSubmit} id="main-form">
        <label>Latitude</label>
        <input
            required
            type="number"
            name="latitude"
            placeholder="Enter your coordinates"
            value={latitude ?? ""}
            onChange={handleLatitudeChange}
        />
        <label>Longitude</label>
        <input
            required
            type="number"
            name="longitude"
            placeholder="Enter your coordinates"
            value={longitude ?? ""}
            onChange={handleLongitudeChange}
        />
        <input type="submit" name="action" value="Submit" className="submit-button"/>
    </form>
    );
};

export default InputForm;

