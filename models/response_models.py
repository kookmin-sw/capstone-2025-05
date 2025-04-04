from pydantic import BaseModel
from typing import Optional, Any, Dict, Union


class StandardResponse(BaseModel):
    """표준 API 응답 모델"""
    success: bool
    message: str
    data: Optional[Any] = None


class ErrorResponse(BaseModel):
    """에러 응답 모델"""
    success: bool = False
    message: str
    error_code: str
    details: Optional[Dict[str, Any]] = None