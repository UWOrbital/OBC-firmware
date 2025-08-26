
# MCC Frontend Dockerfile (Deno + Vite.js)
FROM denoland/deno:alpine
WORKDIR /app
COPY . /app/

# Install npm dependencies using Deno's npm compatibility
RUN deno task npm:install || deno run -A npm:install || true

EXPOSE 3001

# Start Vite dev server using Deno
CMD ["deno", "task", "dev", "--host", "0.0.0.0","--port", "3001"]
