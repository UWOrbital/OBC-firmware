import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";
import tsconfigPaths from "vite-tsconfig-paths";
import deno from "@deno/vite-plugin";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react(), tsconfigPaths(), deno()],
});
