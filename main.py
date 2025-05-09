from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware

from app.routers import songs

app = FastAPI(
    title="Maple Media Server",
    description="음악 및 미디어 데이터를 위한 서버",
    version="0.1.0",
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

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True) 
