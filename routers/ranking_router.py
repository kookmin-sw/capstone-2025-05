from fastapi import APIRouter, HTTPException
from firebase_admin import db

router = APIRouter()

@router.get("/rank", tags=["Rank"])
async def get_rank(song_name: str):
    try:
        rank_ref = db.reference(f"/rank/{song_name}")
        rank_data = rank_ref.get()
        
        if not rank_data:
            raise HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")
        
        scores = []
        for score, nickname in rank_data.items():
            try:
                scores.append({"nickname": nickname, "score": int(score)})
            except ValueError:
                print(f"잘못된 점수 형식: {score}")
                continue
        
        scores.sort(key=lambda x: x["score"], reverse=True)
        
        ranking = []
        for idx, entry in enumerate(scores[:100], start=1):
            ranking.append({
                "rank": idx,
                "nickname": entry["nickname"],
                "score": entry["score"]
            })
        
        return {"ranking": ranking}
    except Exception as e:
        print(f"랭킹 데이터 조회 실패: {str(e)}")
        raise HTTPException(status_code=500, detail="랭킹 데이터 조회에 실패했습니다.")