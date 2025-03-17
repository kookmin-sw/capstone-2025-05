from fastapi import FastAPI, HTTPException
from routers.myPage_router import router as myPage_router
from manager.firebase_manager import db

app = FastAPI()

app.include_router(myPage_router) 

@app.post("/test")
async def test():
    try:
        doc_ref = db.collection("test").document()
        doc_ref.set({
            "이름": "김개똥",
            "기타 실력": "1"
        })
        return {"message": "Data added successfully", "doc_id": doc_ref.id}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))