# MCC Frontend Dockerfile (Node.js)
FROM node:20.19-alpine
WORKDIR /app
COPY . /app/
EXPOSE 3001
CMD ["deno", "run", "dev", "--port", "3001"]
