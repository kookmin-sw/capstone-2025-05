from fastapi import FastAPI, HTTPException, Depends
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles

from manager.test_service import TestService
from models.response_models import StandardResponse, ErrorResponse
from routers.account_router import router as account_router
from routers.post_router import router as post_router
from routers.mypage_router import router as mypage_router
from routers.spotify_router import router as spotify_router
from routers.recent_prepare_router import router as recent_prepare_router
from routers.ranking_router import router as ranking_router
from routers.mainsearch_router import router as mainsearch_router
from routers.soundSourceAnalysis_router import router as sound_source_analysis_router

app = FastAPI(
    title="MAPLE API Server",
    description="Music Analysis & Performance Learning Environment - 음악 분석 및 학습 환경을 위한 API 서버",
    version="1.0.0"
)

origins = [
    "http://localhost:3000",
    "https://localhost:3000",
    "https://maple.ne.kr"
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.mount("/static", StaticFiles(directory="static"), name="static")

app.include_router(account_router, prefix="/account", tags=["Account"])
app.include_router(post_router, prefix="/post", tags=["Post"])
app.include_router(mypage_router, prefix="/mypage", tags=["MyPage"])
app.include_router(spotify_router, prefix="/api/spotify", tags=["Spotify"])
app.include_router(recent_prepare_router, prefix="/api/prepare", tags=["RecentPrepare"])
app.include_router(ranking_router, prefix="/ranking", tags=["Ranking"])
app.include_router(mainsearch_router, prefix="/search", tags=["Search"])
app.include_router(sound_source_analysis_router, prefix="/analysis", tags=["Analysis"])

@app.get("/")
async def root():
    return StandardResponse(
        success=True,
        message="API 서버가 정상적으로 실행 중입니다",
        data={"status": "online"}
    )

@app.post("/test", response_model=StandardResponse)
async def test():
    try:
        doc_result = await TestService.create_test_document("김개똥", "1")
        file_result = await TestService.upload_sample_file("sample.txt", "uploads/sample.txt")
        return StandardResponse(
            success=True,
            message="테스트가 성공적으로 완료되었습니다",
            data={
                "document": doc_result,
                "file": file_result
            }
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=f"서버 오류: {str(e)}"
        )

@app.exception_handler(HTTPException)
async def http_exception_handler(request, exc):
    return JSONResponse(
        status_code=exc.status_code,
        content=ErrorResponse(
            message="요청 처리 중 오류가 발생했습니다",
            error_code=f"HTTP_{exc.status_code}",
            details={"detail": exc.detail}
        ).dict()
    )

@app.exception_handler(Exception)
async def general_exception_handler(request, exc):
    return JSONResponse(
        status_code=500,
        content=ErrorResponse(
            message="서버 내부 오류가 발생했습니다",
            error_code="INTERNAL_SERVER_ERROR",
            details={"detail": str(exc)}
        ).dict()
    )

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)