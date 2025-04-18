FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1

WORKDIR /srv

# Install git and build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    build-essential \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

# Copy model file to the models directory
RUN mkdir -p /srv/models
RUN if [ -f "guitar_technique_classifier.keras" ]; then cp guitar_technique_classifier.keras /srv/models/; fi

# FastAPI execution (Gateway)
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]