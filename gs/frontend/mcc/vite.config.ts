import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";
import deno from "@deno/vite-plugin";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react(), deno()],
});
