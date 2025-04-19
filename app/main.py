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

# Create FastAPI app
app = FastAPI(
    title="MAPLE Audio Analysis API",
    description="API for analyzing and comparing audio performances",
    version="0.1.0",
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Modify for production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
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
        "docs_url": "/docs",
    }


@app.get("/health")
async def health_check():
    """Health check endpoint for monitoring."""
    return {"status": "healthy"}


@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    """Global exception handler for unhandled exceptions."""
    return JSONResponse(
        status_code=500,
        content={"detail": "Internal server error", "error": str(exc)},
    )