import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App.tsx";
import "./index.css"; // tailwind css, jaldi font and overflow hidden are imported here
import { BrowserRouter } from "react-router-dom";
import { CSS_VARIABLES } from "./style-const.ts";

// Inject CSS variables from css-const.ts into the document root
Object.entries(CSS_VARIABLES).forEach(([property, value]) => {
  document.documentElement.style.setProperty(property, value);
});

/**
 * @brief Main component displaying the main application, which is linked to the index.html file
 * @return tsx element of Main component
 */
ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <BrowserRouter>
      <App />
    </BrowserRouter>
  </React.StrictMode>
);
