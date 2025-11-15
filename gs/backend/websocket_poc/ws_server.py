import asyncio
import json
from datetime import datetime

import uvicorn
from fastapi import FastAPI, WebSocket, WebSocketDisconnect

app = FastAPI(title="WebSocket Terminal")


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket) -> None:
    """
    Handle a WebSocket connection for the Raspberry Pi terminal.

    Receives commands from the client, executes them asynchronously on
    the Raspberry Pi shell, and streams stdout/stderr back to the client
    in real time, including status messages.
    """

    await websocket.accept()
    print("New connection established.")

    try:
        while True:
            # Receive command from client
            command = await websocket.receive_text()
            print(f"Received command: {command}")

            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            # Step 1: Send loading indicator
            await websocket.send_text(
                json.dumps({"type": "status", "message": "...", "timestamp": timestamp, "source": "raspberry-pi"})
            )

            # Step 2: Run command on Raspberry Pi shell
            process = await asyncio.create_subprocess_shell(
                command, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE
            )

            # Step 3: Read and send output line by line
            while True:
                line = await process.stdout.readline()
                if not line:
                    break
                output = line.decode().strip()
                if output:
                    await websocket.send_text(
                        json.dumps(
                            {
                                "type": "command",
                                "message": output,
                                "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                                "source": "raspberry-pi",
                            }
                        )
                    )

            # Step 4: Capture and send any errors
            err = await process.stderr.read()
            if err:
                await websocket.send_text(
                    json.dumps(
                        {
                            "type": "error",
                            "message": err.decode().strip(),
                            "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                            "source": "raspberry-pi",
                        }
                    )
                )

            # Step 5: Notify completion
            await websocket.send_text(
                json.dumps(
                    {
                        "type": "status",
                        "message": f"Command '{command}' completed.",
                        "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                        "source": "raspberry-pi",
                    }
                )
            )

    except WebSocketDisconnect:
        print("Client disconnected.")
    except Exception as e:
        print(f"Error: {e}")
        await websocket.send_text(
            json.dumps(
                {
                    "type": "error",
                    "message": str(e),
                    "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "source": "server",
                }
            )
        )


if __name__ == "__main__":
    uvicorn.run("backend.websocket_poc.ws_server:app", host="0.0.0.0", port=9067, reload=True)
