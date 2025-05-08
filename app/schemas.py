from enum import Enum
from typing import Dict, List, Optional, Union, Any
from datetime import datetime

from pydantic import BaseModel, Field


class TaskStatus(str, Enum):
    PENDING = "PENDING"
    STARTED = "STARTED"
    PROCESSING = "PROCESSING"  # 추가된 상태
    FINALIZING = "FINALIZING"  # 새로 추가된 상태 (결과 마무리 단계)
    SUCCESS = "SUCCESS"
    FAILURE = "FAILURE"
    REVOKED = "REVOKED"
    REJECTED = "REJECTED"
    RETRY = "RETRY"


class AnalysisType(str, Enum):
    SIMPLE = "simple"
    DETAILED = "detailed"
    COMPARE = "compare"


class TaskResponse(BaseModel):
    task_id: str
    status: str = "PENDING"


class ProgressResponse(BaseModel):
    task_id: str
    status: TaskStatus
    progress: int = Field(0, ge=0, le=100)
    result: Optional[Dict] = None
    error: Optional[str] = None


class AnalysisRequest(BaseModel):
    analysis_type: AnalysisType = AnalysisType.SIMPLE
    user_id: Optional[str] = None
    song_id: Optional[str] = None
    reference_file_id: Optional[str] = None
    midi_file_id: Optional[str] = None
    generate_feedback: bool = False  # GROK 피드백 생성 옵션 추가


class AudioFeatures(BaseModel):
    tempo: float
    onsets: List[float]
    pitches: List[float]
    techniques: List[List[str]]


class NoteComparison(BaseModel):
    note_index: int
    user_timing_deviation: float
    reference_timing_deviation: float
    timing_comparison: str


class ComparisonResult(BaseModel):
    user_features: AudioFeatures
    reference_features: AudioFeatures
    scores: Dict[str, float]
    note_comparisons: Optional[List[NoteComparison]] = None
    
    # 기존 필드들은 하위 호환성을 위해 유지
    tempo_match_percentage: float
    pitch_match_percentage: float
    rhythm_match_percentage: float
    technique_match_percentage: float
    overall_score: float
    
    # 새로 추가된 리듬 관련 필드
    rhythm_absolute_match: Optional[float] = None
    rhythm_relative_match: Optional[float] = None
    expression_similarity: Optional[float] = None


class MidiNote(BaseModel):
    """MIDI 노트 정보"""
    start: float  # 시작 시간 (초)
    end: float  # 종료 시간 (초)
    pitch: int  # MIDI 음높이 (0-127)
    velocity: int  # 세기 (0-127)
    channel: int  # MIDI 채널 (0-15)


class MidiData(BaseModel):
    """MIDI 파일 데이터"""
    notes: List[Dict[str, Any]]
    tempos: List[float]
    tempo_times: List[float]


class ReferenceFeatures(BaseModel):
    """레퍼런스 오디오 특성 데이터"""
    song_id: str
    features: Dict[str, Any]  # AudioFeatures 형식의 데이터
    midi_data: Optional[Dict[str, Any]] = None  # MidiData 형식의 데이터 (선택 사항)
    created_at: Optional[str] = None
    
    class Config:
        schema_extra = {
            "example": {
                "song_id": "song-123",
                "features": {
                    "tempo": 120.5,
                    "onsets": [0.5, 1.0, 1.5, 2.0, 2.5],
                    "pitches": [440.0, 523.25, 587.33, 659.26, 783.99],
                    "techniques": [["regular"], ["hammer-on"], ["pull-off"], ["slide"], ["bend"]]
                },
                "created_at": datetime.now().isoformat()
            }
        }


# MongoDB 스키마 추가
class AnalysisResultResponse(BaseModel):
    """MongoDB에서 가져온 분석 또는 비교 결과를 위한 응답 모델"""
    _id: str
    task_id: str
    result: Dict[str, Any]
    user_id: Optional[str] = None
    song_id: Optional[str] = None
    created_at: Optional[str] = None
    result_type: Optional[str] = None  # "analysis" 또는 "comparison"
    
    class Config:
        """Pydantic 설정"""
        orm_mode = True


class ReferenceFeatureResponse(BaseModel):
    """MongoDB에서 가져온 레퍼런스 오디오 특성을 위한 응답 모델"""
    _id: str
    song_id: str
    features: Dict[str, Any]
    midi_data: Optional[Dict[str, Any]] = None
    created_at: Optional[str] = None
    
    class Config:
        """Pydantic 설정"""
        orm_mode = True