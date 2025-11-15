import React, { useEffect, useRef, useState } from "react";

interface Message {
  text: string;
  timestamp: string;
  sender: "user" | "server";
}

const WebsocketTerminal: React.FC = () => {
  const [socket, setSocket] = useState<WebSocket | null>(null);
  const [input, setInput] = useState("");
  const [messages, setMessages] = useState<Message[]>([]);
  const messagesEndRef = useRef<HTMLDivElement>(null);
  const inputRef = useRef<HTMLInputElement>(null);

  // Connect to WebSocket backend
  useEffect(() => {
    // Adjust for connection when connecting to raspberry pi
    const ws = new WebSocket("ws://localhost:9067/ws");
    setSocket(ws);

    ws.onopen = () => {
      console.log("Connected to WebSocket backend");
    };

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        const { type, message, timestamp } = data;

        if (type === "command" || type === "status" || type === "error") {
          setMessages((prev) => [
            ...prev,
            { text: message, timestamp, sender: "server" },
          ]);
        }
      } catch {
        // fallback for plain text
        setMessages((prev) => [
          ...prev,
          {
            text: event.data,
            timestamp: new Date().toLocaleString(),
            sender: "server",
          },
        ]);
      }
    };

    ws.onclose = () => console.log("WebSocket disconnected");

    return () => ws.close();
  }, []);

  // Auto-scroll when new messages come in
  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  // Send command to Raspberry Pi
  const sendCommand = () => {
    if (socket && input.trim() !== "") {
      const timestamp = new Date().toLocaleString();
      socket.send(input);

      setMessages((prev) => [
        ...prev,
        { text: input, timestamp, sender: "user" },
      ]);

      setInput("");
      setTimeout(() => inputRef.current?.focus(), 50);
    }
  };

  const handleKeyPress = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === "Enter") sendCommand();
  };

  const formatDateTime = (timestamp: string) => {
    const date = new Date(timestamp);
    const day = date.getDate();
    const month = date.toLocaleString("default", { month: "short" });
    const year = date.getFullYear();
    const time = date.toLocaleTimeString();
    return `${day} ${month} ${year} ${time}`;
  };

  return (
    <div
      className="flex flex-col mt-[10vh] h-[70vh] w-[80vw] mx-auto rounded-lg
                 shadow-2xl bg-black/45 backdrop-blur-md text-white font-mono text-xs
                 border border-gray-700"
    >
      {/* Message Output */}
      <div className="flex-1 overflow-y-auto p-3">
        {messages.map((msg, idx) => (
          <div key={idx} className="whitespace-pre-wrap leading-relaxed mb-1">
            <span className="text-gray-500">
              [{formatDateTime(msg.timestamp)}]
            </span>{" "}
            {msg.sender === "user" ? (
              <>
                <span className="text-green-500">user@pi</span>:~$
                <span className="ml-2">{msg.text}</span>
              </>
            ) : (
              <>
                <span className="text-yellow-400">pi&gt;</span>{" "}
                <span>{msg.text}</span>
              </>
            )}
          </div>
        ))}
        <div ref={messagesEndRef} />
      </div>

      {/* Input Bar */}
      <div className="flex items-center p-3 border-t border-gray-700 bg-black/70 backdrop-blur-md">
        <span className="text-green-500 pr-2">user@pi:~$</span>
        <input
          ref={inputRef}
          className="flex-1 bg-transparent text-white px-2 py-1 outline-none border-none font-mono placeholder-gray-600 text-xs"
          type="text"
          placeholder="Enter command..."
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyDown={handleKeyPress}
          autoFocus
        />
      </div>
    </div>
  );
};

export default WebsocketTerminal;