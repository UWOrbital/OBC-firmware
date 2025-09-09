
# ARO Frontend Dockerfile (Deno + Vite.js)
FROM denoland/deno:alpine
WORKDIR /app
COPY . /app/

RUN deno install

EXPOSE 3000

# Start Vite dev server using Deno
CMD ["deno", "task", "dev", "--host", "0.0.0.0","--port", "3000"]
