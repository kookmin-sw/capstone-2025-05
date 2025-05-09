from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.responses import JSONResponse
from starlette.middleware.base import BaseHTTPMiddleware

from app.routers import songs
from app.database import engine, Base
from app.models.song import SongModel
from sqlalchemy.orm import Session
from app.database import SessionLocal

# 간단한 HSTS 미들웨어
class HTTPSHeadersMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request: Request, call_next):
        response = await call_next(request)
        # HSTS 헤더 추가
        response.headers["Strict-Transport-Security"] = "max-age=31536000; includeSubDomains"
        return response

app = FastAPI(
    title="Maple Media Server",
    description="음악 및 미디어 데이터를 위한 서버",
    version="0.1.0",
    # API 문서 URL 명시적 설정 - /api/v1 접두사 추가
    docs_url="/api/v1/docs",
    redoc_url="/api/v1/redoc",
    openapi_url="/api/v1/openapi.json",
    # 슬래시 리다이렉션 비활성화
    redirect_slashes=False
)

# HTTPS 헤더 미들웨어 추가
app.add_middleware(HTTPSHeadersMiddleware)

# 정적 파일 서빙 설정
app.mount("/static", StaticFiles(directory="app/static"), name="static")

# 라우터 등록 - /api/v1 접두사 추가
app.include_router(songs.router, prefix="/api/v1/songs")

@app.get("/")
async def root():
    return {
        "name": "Maple Media Server",
        "version": "0.1.0",
        "status": "online",
        "documentation": "/api/v1/docs",
        "endpoints": {
            "songs": "/api/v1/songs"
        }
    }

@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    """전역 예외 핸들러"""
    return JSONResponse(
        status_code=500,
        content={"detail": "Internal server error", "error": str(exc)},
    )

@app.on_event("startup")
async def startup_event():
    """서버 시작 시 실행되는 이벤트 핸들러"""
    import fastapi
    print(f"FastAPI 버전: {fastapi.__version__}")
    print(f"서버 URL: {app.root_path}")
    print(f"OpenAPI URL: {app.openapi_url}")
    print(f"Docs URL: {app.docs_url}")
    
    # 데이터베이스 테이블 생성
    Base.metadata.create_all(bind=engine)
    
    # 테스트 데이터 초기화
    db = SessionLocal()
    try:
        songs.create_test_data(db)
    finally:
        db.close()

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True) 
