from pydantic import BaseModel
import datetime
from typing import Optional

class Post(BaseModel):
    uid: str
    게시판종류이름: str
    내용: str
    댓글갯수: int = 0
    비밀번호: str
    작성일시: datetime.datetime
    작성자: str
    제목: str
    조회수: int = 0
    좋아요수: int = 0

    class Config:
        orm_mode = True  # SQLAlchemy와 호환될 수 있도록 설정 (기본적으로 DB에서 쿼리한 데이터를 Pydantic 모델로 변환)

class Comment(BaseModel):
    uid: str
    postid: int
    내용: str
    비밀번호: str
    작성자: str
    프로필이미지: Optional[str] = None

    class Config:
        orm_mode = True  # SQLAlchemy와 호환될 수 있도록 설정 (기본적으로 DB에서 쿼리한 데이터를 Pydantic 모델로 변환)
