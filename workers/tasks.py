import os
import tempfile
from celery import Celery
from celery.utils.log import get_task_logger
import json

from workers.dsp import analyze_simple, compare_audio_with_reference

# Initialize Celery app
celery_app = Celery('maple_audio_analyzer')
celery_app.config_from_object('celeryconfig')
logger = get_task_logger(__name__)


@celery_app.task(bind=True, name='workers.tasks.analyze_audio')
def analyze_audio(self, audio_bytes, request_dict=None):
    """
    Celery task to analyze audio content.
    
    Parameters:
    - audio_bytes: Binary content of the uploaded audio file
    - request_dict: Dictionary with additional analysis parameters
    
    Returns:
    - Dictionary with analysis results
    """
    logger.info(f"Starting audio analysis task {self.request.id}")
    self.update_state(state='STARTED', meta={'progress': 10})
    
    try:
        # Process the request dictionary if provided
        if request_dict is None:
            request_dict = {}
        
        analysis_type = request_dict.get('analysis_type', 'simple')
        user_id = request_dict.get('user_id')
        song_id = request_dict.get('song_id')
        
        # Update progress
        self.update_state(state='PROCESSING', meta={'progress': 30})
        
        # Perform analysis
        result = analyze_simple(audio_bytes)
        
        # Update progress
        self.update_state(state='FINALIZING', meta={'progress': 90})
        
        # Add metadata to result
        result['metadata'] = {
            'user_id': user_id,
            'song_id': song_id,
            'analysis_type': analysis_type,
            'task_id': self.request.id
        }
        
        logger.info(f"Audio analysis task {self.request.id} completed successfully")
        return result
        
    except Exception as e:
        logger.exception(f"Error in audio analysis task: {str(e)}")
        raise


@celery_app.task(bind=True, name='workers.tasks.compare_audio')
def compare_audio(self, user_audio_bytes, reference_audio_bytes=None, midi_bytes=None, user_id=None, song_id=None):
    """
    Celery task to compare user audio with reference audio and/or MIDI.
    
    Parameters:
    - user_audio_bytes: Binary content of the user's audio recording
    - reference_audio_bytes: Binary content of the reference audio (optional)
    - midi_bytes: Binary content of the MIDI file (optional)
    - user_id: ID of the user who uploaded the audio (optional)
    - song_id: ID of the song being analyzed (optional)
    
    Returns:
    - Dictionary with comparison results
    """
    logger.info(f"Starting audio comparison task {self.request.id}")
    self.update_state(state='STARTED', meta={'progress': 10})
    
    try:
        if reference_audio_bytes is None:
            # If no reference audio is provided, just perform simple analysis
            self.update_state(state='PROCESSING', meta={'progress': 50})
            result = analyze_simple(user_audio_bytes)
            
        else:
            # Compare user audio with reference
            self.update_state(state='PROCESSING', meta={'progress': 30})
            result = compare_audio_with_reference(user_audio_bytes, reference_audio_bytes, midi_bytes)
        
        # Update progress
        self.update_state(state='FINALIZING', meta={'progress': 90})
        
        # Add metadata to result
        result['metadata'] = {
            'user_id': user_id,
            'song_id': song_id,
            'task_id': self.request.id,
            'has_reference': reference_audio_bytes is not None,
            'has_midi': midi_bytes is not None
        }
        
        logger.info(f"Audio comparison task {self.request.id} completed successfully")
        return result
        
    except Exception as e:
        logger.exception(f"Error in audio comparison task: {str(e)}")
        raise