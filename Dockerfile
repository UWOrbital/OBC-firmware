# Backend Dockerfile
FROM python:3.11-slim
WORKDIR /app
COPY requirements.txt /app/
COPY /gs/backend /app/
RUN pip install -r requirements.txt
EXPOSE 8000

CMD ["fastapi", "run", "gs/backend/main.py"]
