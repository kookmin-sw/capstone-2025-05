from fastapi import HTTPException
from firebase_admin import db, storage
import urllib.parse

class RankingService:
    """랭킹 관련 서비스 클래스"""
    
    @staticmethod
    async def get_rank(song_name: str):
        """
        특정 곡의 랭킹 정보를 가져옵니다.
        
        Args:
            song_name: 랭킹을 조회할 곡 이름
            
        Returns:
            랭킹 목록 (최대 100명)
            
        Raises:
            HTTPException: 랭킹 정보가 없을 경우
        """
        decoded_song_name = urllib.parse.unquote(song_name)
        
        ref = db.reference(f"/rank/{decoded_song_name}")
        data = ref.get()
        
        if not data:
            raise HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")
        
        rankings = []
        for uid, info in data.items():
            if isinstance(info, dict) and "score" in info and "nickname" in info:
                rankings.append({"uid": uid, "nickname": info["nickname"], "score": info["score"]})
        
        # 점수 내림차순, 동점일 경우 닉네임 오름차순으로 정렬
        rankings.sort(key=lambda x: (-x["score"], x["nickname"]))
        rankings = rankings[:100]  # 상위 100명만 표시
        
        # 상위 3명에게 프로필 이미지 URL 추가
        for i, entry in enumerate(rankings[:3]):
            try:
                bucket = storage.bucket()
                blob = bucket.blob(f"{entry['uid']}/profile")
                rankings[i]["profile_url"] = blob.generate_signed_url(expiration=3600)
            except Exception:
                rankings[i]["profile_url"] = None
        
        # 순위 정보 추가
        for i, entry in enumerate(rankings):
            entry["rank"] = i + 1
        
        return rankings