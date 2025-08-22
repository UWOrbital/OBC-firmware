# ARO Frontend Dockerfile (Node.js)
FROM node:20.19-alpine
WORKDIR /app
COPY . /app/
EXPOSE 3000
CMD ["deno", "run", "dev", "--port", "3000"]
