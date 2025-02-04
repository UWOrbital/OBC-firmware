import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import deno from "@deno/vite-plugin";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react(), deno()],
});
