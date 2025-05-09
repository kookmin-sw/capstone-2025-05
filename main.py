from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.responses import JSONResponse
from starlette.middleware.base import BaseHTTPMiddleware

from app.routers import songs

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
    # API 문서 URL 명시적 설정
    docs_url="/docs",
    redoc_url="/redoc",
    openapi_url="/openapi.json"
)

# HTTPS 헤더 미들웨어 추가
app.add_middleware(HTTPSHeadersMiddleware)

# 정적 파일 서빙 설정
app.mount("/static", StaticFiles(directory="app/static"), name="static")

# 라우터 등록 - 명시적 경로 지정
app.include_router(songs.router, prefix="/songs")

@app.get("/")
async def root():
    return {
        "message": "Maple Media Server에 오신 것을 환영합니다!",
        "documentation": "/docs",
        "endpoints": {
            "songs": "/songs"
        }
    }

@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    """전역 예외 핸들러"""
    return JSONResponse(
        status_code=500,
        content={"detail": "서버 내부 오류", "error": str(exc)},
    )

@app.on_event("startup")
async def startup_event():
    """서버 시작 시 실행되는 이벤트 핸들러"""
    import fastapi
    print(f"FastAPI 버전: {fastapi.__version__}")
    print(f"서버 URL: {app.root_path}")
    print(f"OpenAPI URL: {app.openapi_url}")
    print(f"Docs URL: {app.docs_url}")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True) 
