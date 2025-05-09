FROM python:3.9-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

# 정적 파일용 디렉토리 확인
RUN mkdir -p app/static/thumbnails app/static/sheet_music app/static/audio

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"] 