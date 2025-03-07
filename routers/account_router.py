from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
import os
import requests
from firebase_admin import auth

router = APIRouter()

GOOGLE_OAUTH2_URL = "https://accounts.google.com/o/oauth2/auth"
GOOGLE_TOKEN_URL = "https://oauth2.googleapis.com/token"
GOOGLE_USERINFO_URL = "https://www.googleapis.com/oauth2/v3/userinfo"

@router.get("/google-login")
async def google_login():
    redirect_uri = os.getenv('GOOGLE_REDIRECT_URI', 'http://localhost:8000/google-auth-callback') 
    google_auth_url = (
        f"{GOOGLE_OAUTH2_URL}?response_type=code&"
        f"client_id={os.getenv('CLIENT_ID')}&"
        f"redirect_uri={redirect_uri}&" 
        f"scope=openid profile email"
    )
    return RedirectResponse(url=google_auth_url) # 지우기 X

@router.get("/google-auth-callback")
async def google_auth_callback(code: str):
    print(f"Received Google auth code: {code}")
    try:
        response = requests.post(
            GOOGLE_TOKEN_URL,
            data={
                "code": code,
                "client_id": os.getenv("CLIENT_ID"),
                "client_secret": os.getenv("CLIENT_SECRET"),
                "redirect_uri": os.getenv("GOOGLE_REDIRECT_URI", "http://localhost:8000/google-auth-callback"),
                "grant_type": "authorization_code",
            },
        )
        response.raise_for_status()

        tokens = response.json()
        print(f"Google Tokens: {tokens}")
        id_token = tokens.get("id_token")

        if not id_token:
            raise HTTPException(status_code=400, detail="No id_token found in Google response.")

        firebase_response = requests.post(
            f"https://identitytoolkit.googleapis.com/v1/accounts:signInWithIdp?key={os.getenv('API_KEY')}",
            headers={"Content-Type": "application/json"},
            json={
                "postBody": f"id_token={id_token}&providerId=google.com",
                "requestUri": "http://localhost",
                "returnIdpCredential": True,
                "returnSecureToken": True,
            }
        )
        
        firebase_response.raise_for_status()
        firebase_data = firebase_response.json()
        
        firebase_id_token = firebase_data.get("idToken")
        if not firebase_id_token:
            raise HTTPException(status_code=400, detail="Failed to obtain Firebase ID token.")

        decoded_token = auth.verify_id_token(firebase_id_token)
        uid = decoded_token["uid"]
        
        user = auth.get_user(uid)

        return {"message": "Google login successful", "user": user.uid}

    except requests.exceptions.RequestException as e:
        raise HTTPException(status_code=400, detail=f"Failed to authenticate with Google: {str(e)}")
    except auth.InvalidIdTokenError:
        raise HTTPException(status_code=400, detail="Invalid token from Google.")