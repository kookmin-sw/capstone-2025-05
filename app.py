from fastapi import FastAPI
from routers.account_router import router as account_router
from routers.posting_router import router as posting_router
from routers.myPage_router import router as myPage_router
from routers.spotify_router import router as spotify_router
from routers.recent_prepare_router import router as recent_prepared_router
from routers.ranking_router import router as ranking_router
from routers.mainsearch_router import router as mainsearch_router
from routers.soundSourceAnalysis_router import router as soundSourceAnalysis_router
from routers.users_router import router as users_router
from routers.analysis_router import router as analysis_router

# API 경로와 문서 설정
app = FastAPI(
    docs_url="/api/docs", 
    redoc_url="/api/redoc",
    openapi_url="/api/openapi.json"
)

# 각 라우터에 /api 접두사 추가
app.include_router(posting_router, prefix="/api")
app.include_router(spotify_router, prefix="/api")
app.include_router(account_router, prefix="/api")
app.include_router(myPage_router, prefix="/api")
app.include_router(ranking_router, prefix="/api")
app.include_router(recent_prepared_router, prefix="/api")
app.include_router(mainsearch_router, prefix="/api")
app.include_router(soundSourceAnalysis_router, prefix="/api")
app.include_router(users_router, prefix="/api")
app.include_router(analysis_router, prefix="/api")