from enum import Enum
from typing import Dict, List, Optional, Union

from pydantic import BaseModel, Field


class TaskStatus(str, Enum):
    PENDING = "PENDING"
    STARTED = "STARTED"
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


class AudioFeatures(BaseModel):
    tempo: float
    onsets: List[float]
    pitches: List[float]
    techniques: List[List[str]]


class ComparisonResult(BaseModel):
    user_features: AudioFeatures
    reference_features: AudioFeatures
    tempo_match_percentage: float
    pitch_match_percentage: float
    rhythm_match_percentage: float
    technique_match_percentage: float
    overall_score: float