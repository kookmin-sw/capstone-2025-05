from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.responses import JSONResponse

from app.routers import songs

app = FastAPI(
    title="Maple Media Server",
    description="음악 및 미디어 데이터를 위한 서버",
    version="0.1.0",
    # root_path를 설정하여 프록시 환경에서 올바른 URL을 생성하도록 함
    root_path="https://media.maple.ne.kr",
    # API 문서 URL 명시적 설정
    docs_url="/docs",
    redoc_url="/redoc",
    openapi_url="/openapi.json"
)

# 정적 파일 서빙 설정
app.mount("/static", StaticFiles(directory="app/static"), name="static")

# 라우터 등록
app.include_router(songs.router)

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

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True) 
