import { describe, it, expect } from "vitest";
import { render, screen } from "@testing-library/react";
import { BrowserRouter } from "react-router-dom";
import "@testing-library/jest-dom";
import Nav from "./Nav";

describe("Nav", () => {
  it("renders logo", () => {
    render(
      <BrowserRouter>
        <Nav />
      </BrowserRouter>
    );
    expect(screen.getByAltText("orbital logo")).toBeInTheDocument();
  });

  it("renders navigation links", () => {
    render(
      <BrowserRouter>
        <Nav />
      </BrowserRouter>
    );
    expect(screen.getByText("Dashboard")).toBeInTheDocument();
    expect(screen.getByText("Commands")).toBeInTheDocument();
    expect(screen.getByText("ARO Admin")).toBeInTheDocument();
    expect(screen.getByText("Live Sessions")).toBeInTheDocument();
    expect(screen.getByText("Login")).toBeInTheDocument();
  });
});
