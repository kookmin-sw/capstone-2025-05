from fastapi import APIRouter, HTTPException
from firebase_admin import auth, db
from manager.firebase_manager import firestore_db, storage_bucket

router = APIRouter()