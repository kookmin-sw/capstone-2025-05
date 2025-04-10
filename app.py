from fastapi import FastAPI, HTTPException
from manager.firebase_manager import firestore_db, storage_bucket, db
from routers.ranking_router import router as ranking_router
from routers.account_router import router as account_router

app = FastAPI()

app.include_router(ranking_router)
app.include_router(account_router) 

@app.post("/test")
async def test():
    try:
        doc_ref = firestore_db.collection("test").document()
        doc_ref.set({
            "이름": "김개똥",
            "기타 실력": "1"
        })

        blob = storage_bucket.blob("uploads/sample.txt")
        blob.upload_from_filename("sample.txt")
        print("성공")

    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e)) 