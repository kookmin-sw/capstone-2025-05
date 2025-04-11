from fastapi import FastAPI
from routers.account_router import router as account_router
# 게시판 import
from routers.myPage_router import router as myPage_router
from routers.spotify_router import router as spotify_router
from routers.recent_prepare_router import router as recent_prepared_router
from routers.ranking_router import router as ranking_router
from routers.mainsearch_router import router as mainsearch_router

app = FastAPI()

app.include_router(account_router)
#app.include_router(post_router)
app.include_router(myPage_router)
app.include_router(spotify_router)
app.include_router(ranking_router)
app.include_router(recent_prepared_router)
app.include_router(mainsearch_router)