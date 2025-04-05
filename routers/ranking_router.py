from fastapi import APIRouter, HTTPException
from firebase_admin import db, auth, storage
import urllib.parse

router = APIRouter()

@router.get("/rank", tags=["Rank"])
async def get_rank(song_name: str):
    decoded_song_name = urllib.parse.unquote(song_name)
    
    ref = db.reference(f"/rank/{decoded_song_name}")
    data = ref.get()
    
    if not data:
        raise HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")
    
    rankings = []
    for uid, info in data.items():
        if isinstance(info, dict) and "score" in info and "nickname" in info:
            rankings.append({"uid": uid, "nickname": info["nickname"], "score": info["score"]})
    
    rankings.sort(key=lambda x: (-x["score"], x["nickname"]))
    rankings = rankings[:100]
    
    for i, entry in enumerate(rankings[:3]):
        try:
            bucket = storage.bucket()
            blob = bucket.blob(f"{entry['uid']}/profile")
            rankings[i]["profile_url"] = blob.generate_signed_url(expiration=3600)
        except Exception:
            rankings[i]["profile_url"] = None
    
    for i, entry in enumerate(rankings):
        entry["rank"] = i + 1
    
    return rankings