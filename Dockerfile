# Backend Dockerfile
FROM python:3.11-slim
WORKDIR /app
COPY requirements.txt /app/
COPY /gs/backend /app/
RUN pip install -r requirements.txt
EXPOSE 8000
# CMD ["uvicorn", "gs.backend.main:app", "--host", "127.0.0.1", "--port", "8000"]
CMD ["fastapi", "run", "gs/backend/main.py"]