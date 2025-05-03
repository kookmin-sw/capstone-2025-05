from fastapi import APIRouter, HTTPException
from firebase_admin import db, storage
import urllib.parse
import re
import datetime

router = APIRouter()

def sanitize_firebase_key(key: str) -> str:
    return re.sub(r'[.#$[\]/?]', '_', key)

@router.get("/rank", tags=["Rank"])
async def get_rank(song_name: str):
    try:
        decoded_song_name = urllib.parse.unquote(song_name)
        safe_song_name = sanitize_firebase_key(decoded_song_name)

        ref = db.reference(f"/rank/{safe_song_name}")
        data = ref.get()

        if not data:
            raise HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")

        rankings = []
        for uid, info in data.items():
            if isinstance(info, dict) and "score" in info and "nickname" in info:
                rankings.append({
                    "uid": uid,
                    "nickname": info["nickname"],
                    "score": info["score"]
                })

        rankings.sort(key=lambda x: (-x["score"], x["nickname"]))

        bucket = storage.bucket()

        for entry in rankings[:3]:
            uid = entry["uid"]

            blobs = list(bucket.list_blobs(prefix=f"{uid}/profile/"))

            if blobs:
                profile_blob = blobs[0]
                url = profile_blob.generate_signed_url(datetime.timedelta(minutes=60))
                entry["profile_url"] = url
            else:
                entry["profile_url"] = None

        for i, entry in enumerate(rankings):
            entry["rank"] = i + 1

        return rankings[:100]

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"랭킹 조회 실패: {str(e)}")