import os
from dotenv import load_dotenv
import firebase_admin
from firebase_admin import credentials, firestore, auth, db, storage

dotenv_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".env"))
load_dotenv(dotenv_path)
# Windows와 macOS/Linux에서 모두 호환되도록 절대 경로 변환
file_path = os.path.abspath(os.getenv("FILE_PATH", "./data/file.txt"))

# Firebase 환경 변수 로드
private_key = os.getenv("PRIVATE_KEY", "").replace('\\n', '\n')
firebase_config = {
    "type": os.getenv("TYPE"),
    "project_id": os.getenv("PROJECT_ID"),
    "private_key_id": os.getenv("PRIVATE_KEY_ID"),
    "private_key": private_key,
    "client_email": os.getenv("CLIENT_EMAIL"),
    "client_id": os.getenv("CLIENT_ID"),
    "auth_uri": os.getenv("AUTH_URI"),
    "token_uri": os.getenv("TOKEN_URI"),
    "auth_provider_x509_cert_url": os.getenv("AUTH_PROVIDER_CERT_URL"),
    "client_x509_cert_url": os.getenv("CLIENT_CERT_URL")
}

if not firebase_admin._apps:
    cred = credentials.Certificate(firebase_config)
    firebase_admin.initialize_app(cred, {
        "databaseURL": os.getenv("DATABASE_URL"),
        "storageBucket": os.getenv("STORAGE_BUCKET")
    })

firestore_db = firestore.client() 
realtime_db = db.reference("/") 
storage_bucket = storage.bucket()