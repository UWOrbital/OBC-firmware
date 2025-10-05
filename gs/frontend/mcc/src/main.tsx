import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App.tsx";
import "./index.css"; // default styles are applied here
import { BrowserRouter } from "react-router-dom";
import store from './store/store.ts'
import { Provider } from 'react-redux'

/**
 * @brief Main component displaying the main application, which is linked to the index.html file
 * @return tsx element of Main component
 */
ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <Provider store={store}>
      <BrowserRouter>
        <App />
      </BrowserRouter>
    </Provider>
  </React.StrictMode>
);
