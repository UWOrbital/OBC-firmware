import InputGroup from "react-bootstrap/InputGroup";
import Form from "react-bootstrap/Form";
import Table from "react-bootstrap/Table";
import { useEffect, useState, useRef } from "react";

const MISSON_COMMAND_PREFIX = "MCC_";

function MissionCommands() {
	const [commands, setCommands] = useState("");
	const [commandResponse, setCommandResponse] = useState("");
	const inputRef = useRef(null);

	useEffect(() => {
		let command = localStorage.getItem(MISSON_COMMAND_PREFIX + "command");
		let response = localStorage.getItem(MISSON_COMMAND_PREFIX + "response");
		if (!command) return;
		if (!response) return;
		setCommands(command);
		setCommandResponse(response);
	}, []);

	// const handleChange = (event) => {
	//   setCommands(event.target.value);
	//   localStorage.setItem(MISSON_COMMAND_PREFIX + "command", event.target.value);
	// }

	const handleCommand = () => {
		const requestOptions = {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ command: commands }),
		};
		fetch("http://localhost:5000/mission-control", requestOptions)
			.then((response) => response.json())
			.then((data) => {
				setCommandResponse(data.response);
				localStorage.setItem(MISSON_COMMAND_PREFIX + "response", data.response);
			});
	};

	const clear = () => {
		localStorage.clear();
		setCommands("");
		setCommandResponse("");
		// inputRef.current.value = ''
	};

	return (
		<div className="missionCommands layout">
			<InputGroup size="sm" className="mb-3">
				<InputGroup.Text id="inputGroup-sizing-sm" onClick={handleCommand}>
					Enter Command
				</InputGroup.Text>
				<Form.Control
					aria-label="Small"
					aria-describedby="inputGroup-sizing-sm"
					// onChange={handleChange}
					ref={inputRef}
				/>
			</InputGroup>
			<Table striped bordered hover>
				<thead>
					<tr>
						<th>Output: {Date()}</th>
					</tr>
				</thead>
				<tbody>
					<tr>{commandResponse && <td>{commandResponse}</td>}</tr>
				</tbody>
			</Table>
			<button onClick={clear}>Clear</button>
		</div>
	);
}

export default MissionCommands;
