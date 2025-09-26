import InputGroup from "react-bootstrap/InputGroup";
import Form from "react-bootstrap/Form";
import Table from "react-bootstrap/Table";
import { useRef, useState } from "react";
import { useQuery, useMutation } from '@tanstack/react-query';

const MISSON_COMMAND_PREFIX = "MCC_";

function MissionCommands() {
  const inputRef = useRef(null);

  const { data: savedData } = useQuery({
    queryKey: ["missionCommand"],
    queryFn: () => {
      const command = localStorage.getItem(MISSON_COMMAND_PREFIX + "command") || "";
      const response = localStorage.getItem(MISSON_COMMAND_PREFIX + "response") || "";
      return { command, response };
    },
    initialData: {
      command: localStorage.getItem(MISSON_COMMAND_PREFIX + "command") || "",
      response: localStorage.getItem(MISSON_COMMAND_PREFIX + "response") || "",
    },
});

  const [commands, setCommands] = useState(savedData.command);

  // const handleChange = (event) => {
  //   setCommands(event.target.value);
  //   localStorage.setItem(MISSON_COMMAND_PREFIX + "command", event.target.value);
  // }
  const mutation = useMutation({
    mutationFn: async (command: string) => {
      const response = await fetch("http://localhost:5000/mission-control", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ command }),
      });
      if (!response.ok) {
        throw new Error(`Server error: ${response.status}`);
      }
      return response.json();
    },
    onSuccess: (data) => {
      localStorage.setItem(
        MISSON_COMMAND_PREFIX + "response",
        data.response
      );
    },
  });

   const handleCommand = () => {
    mutation.mutate(commands);
  };

  const clear = () => {
    try {
      localStorage.clear();
    } catch (error) {
      console.error(error);
    }
    setCommands("");
    mutation.reset();
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
          <tr>
            <td>
              {mutation.status === "pending" && "Loading..."}
              {mutation.status === "error" && `Error: ${(mutation.error as Error).message}`}
              {mutation.status === "success" && mutation.data.response}
            </td>
          </tr>
        </tbody>
      </Table>
      <button onClick={clear}>Clear</button>
    </div>
  );
}

export default MissionCommands;
