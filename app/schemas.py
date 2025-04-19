from enum import Enum
from typing import Dict, List, Optional, Union

from pydantic import BaseModel, Field


class TaskStatus(str, Enum):
    PENDING = "PENDING"
    STARTED = "STARTED"
    PROCESSING = "PROCESSING"  # 추가된 상태
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