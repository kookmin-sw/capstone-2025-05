import os
import scipy.signal  # SciPy 패치를 위해 추가

# SciPy 호환성 패치 적용
# 최신 버전의 SciPy에서는 scipy.signal.hann이 scipy.signal.windows.hann으로 이동했습니다
if not hasattr(scipy.signal, 'hann') and hasattr(scipy.signal, 'windows') and hasattr(scipy.signal.windows, 'hann'):
    print("패치 적용 (app): scipy.signal.hann -> scipy.signal.windows.hann")
    scipy.signal.hann = scipy.signal.windows.hann

from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse

from app.api import api_router
# MongoDB 모듈 가져오기
from app.db import client as mongo_client

# Create FastAPI app
app = FastAPI(
    title="MAPLE Audio Analysis API",
    description="API for analyzing and comparing audio performances",
    version="0.1.0",
    docs_url="/api/v1/docs", 
    redoc_url="/api/v1/redoc",
    openapi_url="/api/v1/openapi.json"
)

# Include API router
app.include_router(api_router)


@app.get("/")
async def root():
    """Root endpoint that returns API information."""
    return {
        "name": "MAPLE Audio Analysis API",
        "version": "0.1.0",
        "status": "online",
        "docs_url": "/api/v1/docs",  # Updated to match the correct docs URL
    }


@app.get("/health")
async def health_check():
    """Health check endpoint for monitoring."""
    # MongoDB 상태 확인
    try:
        # 서버 정보 요청을 통해 MongoDB 연결 확인
        mongo_client.server_info()
        mongo_status = "connected"
    except Exception as e:
        mongo_status = f"disconnected: {str(e)}"
    
    return {
        "status": "healthy",
        "services": {
            "mongodb": mongo_status
        }
    }


@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    """Global exception handler for unhandled exceptions."""
    return JSONResponse(
        status_code=500,
        content={"detail": "Internal server error", "error": str(exc)},
    )


@app.on_event("startup")
async def startup_db_client():
    """애플리케이션 시작 시 MongoDB에 연결"""
    try:
        # MongoDB 연결 확인
        mongo_client.server_info()
        print("MongoDB 연결 성공")
    except Exception as e:
        print(f"MongoDB 연결 실패: {e}")


@app.on_event("shutdown")
async def shutdown_db_client():
    """애플리케이션 종료 시 MongoDB 연결 종료"""
    mongo_client.close()
    print("MongoDB 연결 종료")