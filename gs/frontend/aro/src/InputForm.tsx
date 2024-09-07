import { useState } from "react";

const InputForm = () => {
	const [latitude, setLatitude] = useState(0);
	const [longitude, setLongitude] = useState(0);
	// const [coordinates, setCoordinates] = useState(latitude());
	const [callSign, setCallSign] = useState("");
	const [name, setName] = useState("");
	const [phoneNumber, setPhoneNumber] = useState("");
	const [email, setEmail] = useState("");

	navigator.geolocation.getCurrentPosition((position) => {
		setLatitude(position.coords.latitude);
		setLongitude(position.coords.longitude);
		// Show a map centered at latitude / longitude.
	});

	const handleSubmit = (event) => {
		const submission = {
			latitude,
			longitude,
			callSign,
			name,
			phoneNumber,
			email,
		};
		console.log(submission);

		alert("Thanks for submitting!");
	};

	return (
		<form onSubmit={handleSubmit} id="main-form">
			<label>Latitude</label>
			<input
				required
				type="text"
				placeholder="Enter your coordinates"
				value={latitude}
				onChange={(event) => setLatitude(event.target.value)}
			/>
			<label>Longitude</label>
			<input
				required
				type="text"
				placeholder="Enter your coordinates"
				value={longitude}
				onChange={(event) => setLongitude(event.target.value)}
			/>
			<label>Call Sign</label>
			<input
				required
				type="text"
				placeholder="Enter your call sign"
				value={callSign}
				onChange={(event) => setCallSign(event.target.value)}
			/>
			<label>Name</label>
			<input
				required
				type="text"
				placeholder="Enter your name"
				value={name}
				onChange={(event) => setName(event.target.value)}
			/>
			<label>Phone Number</label>
			<input
				required
				type="text"
				placeholder="Enter your phone number"
				value={phoneNumber}
				onChange={(event) => setPhoneNumber(event.target.value)}
			/>
			<label>Email</label>
			<input
				required
				type="email"
				placeholder="Enter your email"
				value={email}
				onChange={(event) => setEmail(event.target.value)}
			/>
			<input type="submit" />
		</form>
	);
};

export default InputForm;
