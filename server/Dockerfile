# Stage 1: Build the Vite frontend
FROM node:20-alpine AS frontend
WORKDIR /app/web
COPY web/package.json web/package-lock.json ./
RUN npm ci
COPY web/ ./
RUN npm run build

# Stage 2: Production image with Python + built frontend
FROM python:3.13-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY ota_server.py server.toml ./
COPY --from=frontend /app/web/dist ./web/dist/

# UI manifests and firmware are mounted as volumes
VOLUME /app/ui
VOLUME /app/firmware

EXPOSE 8080
CMD ["python", "ota_server.py", "--config", "server.toml"]
