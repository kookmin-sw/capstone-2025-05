FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1
# TensorFlow 최적화 환경 변수 추가
ENV TF_ENABLE_ONEDNN_OPTS=0

WORKDIR /srv

# 시스템 및 빌드 의존성 설치 (ARM 호환성 강화)
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    build-essential \
    libsndfile1 \
    ffmpeg \
    libhdf5-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .

# 멀티 아키텍처 지원을 위한 pip 설치 최적화
RUN pip install --upgrade pip && \
    pip install --no-cache-dir -r requirements.txt

# NumPy 오류 방지용 심볼릭 링크 (ARM 호환성 확보)
RUN ln -s /usr/lib/aarch64-linux-gnu/libf77blas.so.3 /usr/lib/aarch64-linux-gnu/libf77blas.so || true

COPY . .

# 모델 디렉토리 및 파일 설정
RUN mkdir -p /srv/models
RUN if [ -f "guitar_technique_classifier.keras" ]; then cp guitar_technique_classifier.keras /srv/models/; fi

# FastAPI 실행 - 호환되는 Uvicorn 옵션만 사용
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000", "--limit-concurrency", "100", "--timeout-keep-alive", "300"]