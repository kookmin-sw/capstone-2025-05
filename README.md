# 🎸 MAPLE Audio Analysis Server

MAPLE (Music Analysis and Performance Learning Engine) is a microservice for analyzing and comparing audio performances, specifically targeting musical instrument recordings.

## 📋 Overview

This server provides a FastAPI gateway with Celery workers for CPU/GPU-intensive audio processing tasks. It's designed as a standalone service that can be integrated with desktop applications, web backends, or other services via HTTP API.

### Features

- **Audio Analysis:** Extract tempo, rhythm, pitch, and technique information from audio recordings
- **Performance Comparison:** Compare a user's recording with reference audio/MIDI
- **Asynchronous Processing:** Handle long-running DSP and ML tasks via Celery worker queues
- **Containerized:** Easy deployment with Docker and Docker Compose
- **CI/CD Pipeline:** Automated testing and container building with GitHub Actions

## 🚀 Quick Start

### Prerequisites

- Docker and Docker Compose
- Git

### Running Locally

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/maple-audio-analyzer.git
   cd maple-audio-analyzer
   ```

2. Create an `.env` file based on the example:
   ```bash
   cp .env.example .env
   ```

3. Build and start the services:
   ```bash
   docker compose up --build
   ```

4. The API is now available at http://localhost:8000

### API Documentation

Once running, access the Swagger documentation at:
- http://localhost:8000/docs

## 🧩 Architecture

This project follows a microservice architecture with the following components:

- **FastAPI Gateway (`app/`)**: Handles HTTP requests, file uploads, and WebSocket connections
- **Celery Workers (`workers/`)**: Performs CPU/GPU intensive DSP and ML computations
- **Redis**: Acts as both message broker and result backend for Celery
- **Models (`models/`)**: Contains trained neural network models for audio analysis

### Project Structure

```
maple-audio-analyzer/
├─ app/                      # FastAPI Gateway
│  ├─ api/                   # API routes
│  │  ├─ __init__.py
│  │  └─ v1.py
│  ├─ main.py                # FastAPI application
│  └─ schemas.py             # Pydantic models
├─ workers/                  # Celery Tasks
│  ├─ __init__.py
│  ├─ tasks.py               # Task definitions
│  └─ dsp.py                 # DSP and ML functions
├─ models/                   # Trained models
├─ tests/                    # Test suite
│  └─ test_api.py
├─ Dockerfile
├─ docker-compose.yml
├─ requirements.txt
├─ celeryconfig.py           # Celery configuration
├─ .env.example
├─ .github/workflows/        # CI/CD pipeline
│  └─ ci.yml
└─ README.md
```

## 💻 Development

### Running Tests

```bash
# From inside the container
pytest -v

# Or using docker compose
docker compose run api pytest -v
```

### Adding New Features

1. For new API endpoints, add them to `app/api/v1.py` or create a new versioned router
2. For new audio processing capabilities, add functions to `workers/dsp.py`
3. Register new Celery tasks in `workers/tasks.py`
4. Add tests for your new functionality

## 📊 API Usage Examples

### Analyze Audio

```bash
curl -X POST "http://localhost:8000/api/v1/analyze" \
  -H "accept: application/json" \
  -H "Content-Type: multipart/form-data" \
  -F "file=@guitar_recording.wav" \
  -F "user_id=user123" \
  -F "song_id=song456"
```

Response:
```json
{
  "task_id": "c769e4fd-5b9c-4b49-a343-fb7e1f32e80f"
}
```

### Compare Performances

```bash
curl -X POST "http://localhost:8000/api/v1/compare" \
  -H "accept: application/json" \
  -H "Content-Type: multipart/form-data" \
  -F "user_file=@user_recording.wav" \
  -F "reference_file=@reference_recording.wav" \
  -F "user_id=user123" \
  -F "song_id=song456"
```

### Check Task Status

```bash
curl -X GET "http://localhost:8000/api/v1/tasks/c769e4fd-5b9c-4b49-a343-fb7e1f32e80f" \
  -H "accept: application/json"
```

Response:
```json
{
  "task_id": "c769e4fd-5b9c-4b49-a343-fb7e1f32e80f",
  "status": "SUCCESS",
  "progress": 100,
  "result": {
    "tempo": 120.5,
    "onsets": [0.5, 1.2, 1.8, 2.4],
    "number_of_notes": 4,
    "duration": 3.2,
    "techniques": [["hammer"], ["pull"], ["normal"], ["bend"]]
  }
}
```

## 🔄 CI/CD Pipeline

This project uses GitHub Actions for continuous integration and deployment:

1. **Testing**: All push and PR events trigger automated testing
2. **Docker Build**: Successful builds on the main branch are packaged as Docker images
3. **Registry**: Images are published to GitHub Container Registry (ghcr.io)

## 📝 License

This project is licensed under the MIT License - see the LICENSE file for details.