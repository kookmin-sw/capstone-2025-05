from fastapi import FastAPI, HTTPException
from manager.firebase_manager import db
from routers.posting_router import posting_router
from routers.spotify_router import spotify_router
from templates import templates
app = FastAPI()
app.include_router(posting_router)
app.include_router(spotify_router)

@app.post("/posttest")
async def test():
    try:
        doc_ref = db.collection("posttest").document()
        doc_ref.set({
            "이름": "김개똥",
            "기타 실력": "1"
        })
        return {"message": "Data added successfully", "doc_id": doc_ref.id}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))