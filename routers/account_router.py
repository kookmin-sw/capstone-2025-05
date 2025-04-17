from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
from manager.account_service import AccountService, UserData, UserEmailSignUp
from models.response_models import StandardResponse

router = APIRouter()

@router.get("/google-login", tags=["Account"])
async def google_login():
    """구글 OAuth 로그인을 시작합니다"""
    google_auth_url = AccountService.get_google_auth_url()
    return RedirectResponse(url=google_auth_url)

@router.get("/google-auth-callback", tags=["Account"], response_model=StandardResponse)
async def google_auth_callback(code: str):
    """구글 OAuth 콜백을 처리합니다"""
    try:
        result = await AccountService.process_google_auth(code)
        return StandardResponse(
            success=True,
            message="로그인 성공",
            data=result
        )
    except HTTPException as e:
        # 이미 HTTPException으로 처리된 예외는 그대로 전달
        raise e
    except Exception as e:
        # 예상치 못한 예외 처리
        raise HTTPException(
            status_code=500, 
            detail=f"서버 오류: {str(e)}"
        )

@router.post("/email-sign-up", tags=["Account"], response_model=StandardResponse)
async def email_sign_up(sign_up_data: UserEmailSignUp):
    """이메일로 회원가입합니다"""
    try:
        result = await AccountService.create_email_user(sign_up_data)
        return StandardResponse(
            success=True,
            message="회원가입 성공",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

@router.post("/set-user-info", tags=["Account"], response_model=StandardResponse)
async def set_user_info(user_info: UserData):
    """사용자 정보를 설정합니다"""
    try:
        result = await AccountService.set_user_information(user_info)
        return StandardResponse(
            success=True,
            message="유저 정보 입력 완료",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

@router.post("/email-login", tags=["Account"], response_model=StandardResponse)
async def email_login(user_data: UserEmailSignUp):
    """이메일로 로그인합니다"""
    try:
        result = await AccountService.login_with_email(user_data)
        return StandardResponse(
            success=True,
            message="로그인 성공",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

@router.delete("/delete-user/{uid}", tags=["Account"], response_model=StandardResponse)
async def delete_user(uid: str):
    """사용자 계정을 삭제합니다"""
    try:
        result = await AccountService.delete_user_account(uid)
        return StandardResponse(
            success=True,
            message="사용자 삭제 완료",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")