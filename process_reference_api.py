#!/usr/bin/env python
"""
레퍼런스 오디오 처리 API 스크립트

이 스크립트는 test/ 폴더에 있는 오디오와 MIDI 파일을 API를 통해 업로드하여
레퍼런스 오디오 분석을 요청합니다. UUID 기반의 song_id를 songs API에서 조회하여 사용합니다.
"""

import os
import sys
import time
import json
import requests
import argparse
import logging
from pathlib import Path

# 로깅 설정
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('process_reference_api')

# API 기본 URL
API_URL = "http://localhost:8000"
SONGS_API_URL = "http://maple.ne.kr:8001/songs/"

def get_songs_data():
    """
    곡 정보 API에서 모든 곡 데이터를 가져옵니다.
    
    Returns:
        곡 정보 목록 또는 None (실패 시)
    """
    try:
        response = requests.get(SONGS_API_URL)
        if response.status_code == 200:
            return response.json()
        else:
            logger.error(f"곡 정보 API 호출 실패: {response.status_code} - {response.text}")
            return None
    except Exception as e:
        logger.error(f"곡 정보 API 호출 중 오류 발생: {str(e)}")
        return None

def get_song_id_by_title(title, songs_data=None):
    """
    곡 제목으로 UUID 기반 song_id를 찾습니다.
    
    Args:
        title: 곡 제목
        songs_data: 이미 조회된 곡 정보 데이터 (없으면 새로 조회)
    
    Returns:
        UUID 기반 song_id 또는 None (찾지 못한 경우)
    """
    if songs_data is None:
        songs_data = get_songs_data()
        
    if not songs_data:
        return None
    
    # 제목이 일치하는 곡 검색
    for song in songs_data:
        if song.get('title', '').lower() == title.lower():
            return song.get('song_id')
    
    # 일치하는 곡이 없을 경우
    logger.warning(f"'{title}' 제목을 가진 곡을 찾을 수 없습니다")
    return None

def process_reference_api(audio_path, midi_path=None, song_id=None, title=None, description=None, wait_for_result=True):
    """
    API를 통해 레퍼런스 오디오와 MIDI를 처리합니다.
    
    Args:
        audio_path: 오디오 파일 경로
        midi_path: MIDI 파일 경로 (선택 사항)
        song_id: UUID 기반 곡 ID (제공되지 않으면 title로 조회)
        title: 곡 제목 (song_id가 없을 때 사용)
        description: 곡 설명 (선택 사항)
        wait_for_result: 결과를 기다릴지 여부
    
    Returns:
        API 응답 결과
    """
    # 파일 존재 확인
    audio_path = Path(audio_path)
    if not audio_path.exists():
        logger.error(f"오디오 파일이 존재하지 않습니다: {audio_path}")
        return None
    
    # song_id 확인 및 조회
    if not song_id and title:
        # 제목으로 UUID 조회
        song_id = get_song_id_by_title(title)
        if not song_id:
            logger.error(f"곡 제목 '{title}'에 해당하는 UUID를 찾을 수 없습니다")
            return None
    elif not song_id:
        # 파일 이름을 제목으로 사용하여 UUID 조회
        title = audio_path.stem
        song_id = get_song_id_by_title(title)
        if not song_id:
            logger.error(f"곡 제목 '{title}'에 해당하는 UUID를 찾을 수 없습니다")
            return None
    
    # API 요청 준비
    files = {
        "reference_file": open(audio_path, 'rb')
    }
    
    # MIDI 파일 추가
    if midi_path:
        midi_path = Path(midi_path)
        if midi_path.exists():
            logger.info(f"MIDI 파일 추가: {midi_path}")
            files["midi_file"] = open(midi_path, 'rb')
        else:
            logger.warning(f"MIDI 파일이 존재하지 않습니다: {midi_path}, MIDI 없이 진행합니다.")
    
    # 요청 데이터 (Form 데이터로 전송)
    data = {
        "song_id": song_id
    }
    
    if description:
        data["description"] = description
    
    logger.info(f"레퍼런스 오디오 분석 요청: song_id={song_id} (제목: {title}), MIDI={'있음' if midi_path and Path(midi_path).exists() else '없음'}")
    
    try:
        # API 요청 디버깅 정보 출력
        logger.info(f"API 요청: URL={API_URL}/api/v1/reference, Files={list(files.keys())}, Data={data}")
        
        # API 요청 전송
        url = f"{API_URL}/api/v1/reference"
        logger.info(f"API 요청 URL: {url}")
        response = requests.post(url, files=files, data=data)
        
        # 파일 핸들러 닫기
        for f in files.values():
            f.close()
        
        if response.status_code == 200:
            result = response.json()
            task_id = result.get('task_id')
            logger.info(f"요청 성공: task_id={task_id}")
            
            # 결과를 기다리는 경우
            if wait_for_result and task_id:
                return check_task_status(task_id)
            
            return result
        else:
            # 오류 응답 상세 출력
            logger.error(f"API 요청 실패: {response.status_code} - {response.text}")
            try:
                error_detail = response.json()
                logger.error(f"오류 상세: {json.dumps(error_detail, indent=2)}")
            except Exception:
                pass
            return None
            
    except Exception as e:
        logger.error(f"API 요청 중 오류 발생: {str(e)}")
        return None

def check_task_status(task_id, max_attempts=30, wait_time=3):
    """
    작업 상태를 확인하고 결과를 기다립니다.
    
    Args:
        task_id: 확인할 작업 ID
        max_attempts: 최대 시도 횟수 (기본값: 30회 - 최대 90초)
        wait_time: 각 시도 사이의 대기 시간(초) (기본값: 3초)
    """
    logger.info(f"작업 상태 확인 중... (ID: {task_id})")
    
    last_status = None
    last_progress = None
    
    for i in range(max_attempts):
        try:
            response = requests.get(f"{API_URL}/api/v1/tasks/{task_id}")
            
            if response.status_code == 200:
                status = response.json()
                current_status = status.get('status')
                current_progress = status.get('progress', 0)
                
                logger.info(f"시도 {i+1}/{max_attempts} - 상태: {current_status}, 진행률: {current_progress}%")
                
                # 상태 변경시 상세 로그
                if current_status != last_status or current_progress != last_progress:
                    last_status = current_status
                    last_progress = current_progress
                
                # 작업 완료 시 결과 반환
                if current_status in ['SUCCESS', 'FAILURE']:
                    if current_status == 'SUCCESS':
                        logger.info("작업 성공적으로 완료됨")
                        
                        # 결과 저장
                        filename = f"reference_{task_id[:8]}_{status.get('result', {}).get('song_id', 'unknown')}.json"
                        with open(filename, 'w') as f:
                            json.dump(status.get('result'), f, indent=2)
                        logger.info(f"결과가 {filename}에 저장되었습니다")
                        
                    else:
                        logger.error(f"작업 실패: {status.get('error')}")
                    
                    return status
            else:
                logger.error(f"상태 확인 실패: {response.status_code} - {response.text}")
            
        except Exception as e:
            logger.error(f"상태 확인 중 오류 발생: {str(e)}")
        
        # 마지막 시도가 아니면 대기
        if i < max_attempts - 1:
            time.sleep(wait_time)
    
    logger.warning(f"최대 시도 횟수({max_attempts}회)를 초과했습니다. 작업이 여전히 진행 중일 수 있습니다.")
    return None

def process_all_files():
    """
    test/ref 및 test/midi 폴더의 모든 오디오 및 MIDI 파일을 API를 통해 처리합니다.
    songs API에서 제공하는 목록과 일치하는 파일만 처리합니다.
    """
    # 기본 경로 설정
    ref_dir = Path('test/ref')
    midi_dir = Path('test/midi')
    
    if not ref_dir.exists():
        logger.error(f"참조 오디오 디렉토리가 존재하지 않습니다: {ref_dir}")
        return
    
    # 곡 정보 미리 조회
    songs_data = get_songs_data()
    if not songs_data:
        logger.error("곡 정보를 가져올 수 없어 처리를 중단합니다")
        return
    
    # 파일명과 UUID 매핑 생성 (소문자로 통일하여 비교)
    title_to_uuid_map = {}
    title_to_original = {}  # 원래 대소문자를 보존하기 위한 매핑
    
    for song in songs_data:
        title = song.get('title', '')
        title_lower = title.lower()
        title_to_uuid_map[title_lower] = song.get('song_id')
        title_to_original[title_lower] = title
    
    # 로컬에 존재하는 파일 목록 가져오기
    local_audio_files = list(ref_dir.glob('*.wav'))
    
    # 매핑 정보 출력
    logger.info("\n처리 가능한 파일과 UUID 매핑:")
    logger.info("-" * 80)
    logger.info(f"{'로컬 파일명':<20} {'제목(API)':<20} {'UUID':<36} {'매칭 상태':<10}")
    logger.info("-" * 80)
    
    # 매핑 확인 및 출력
    matched_files = []
    for audio_file in local_audio_files:
        file_stem = audio_file.stem
        file_stem_lower = file_stem.lower()
        
        # API 목록에서 제목과 일치하는 항목 찾기
        if file_stem_lower in title_to_uuid_map:
            uuid = title_to_uuid_map[file_stem_lower]
            original_title = title_to_original[file_stem_lower]
            matched_files.append((audio_file, original_title, uuid))
            logger.info(f"{audio_file.name:<20} {original_title:<20} {uuid:<36} {'✓ 매칭됨':<10}")
        else:
            logger.info(f"{audio_file.name:<20} {'??':<20} {'??':<36} {'✗ 없음':<10}")
    
    logger.info("-" * 80)
    logger.info(f"총 {len(local_audio_files)}개 파일 중 {len(matched_files)}개 매칭됨")
    logger.info("-" * 80)
    
    # 매칭된 파일이 없으면 중단
    if not matched_files:
        logger.info("매칭된 파일이 없어 처리를 중단합니다.")
        return
    
    # 사용자에게 계속 진행할지 확인
    confirm = input("\n위 목록을 확인하고 처리를 진행하시겠습니까? (y/n): ").strip().lower()
    if confirm != 'y':
        logger.info("사용자에 의해 처리가 취소되었습니다.")
        return
    
    # 처리된 파일 수 카운트
    processed = 0
    failed = 0
    
    # 매칭된 파일만 처리
    for audio_file, title, song_id in matched_files:
        # 동일한 이름의 MIDI 파일을 찾음
        midi_file = midi_dir / f"{audio_file.stem}.mid"
        midi_path = midi_file if midi_file.exists() else None
        
        logger.info(f"처리 중: 제목={title}, song_id={song_id}")
        result = process_reference_api(
            audio_path=audio_file,
            midi_path=midi_path,
            song_id=song_id,
            title=title
        )
        
        if result:
            processed += 1
        else:
            failed += 1
    
    logger.info(f"처리 완료: 성공={processed}, 실패={failed}")

def main():
    """
    메인 함수: 명령행 인수를 파싱하고 적절한 작업을 수행합니다.
    """
    parser = argparse.ArgumentParser(description='레퍼런스 오디오 처리 API 스크립트')
    parser.add_argument('--all', action='store_true', help='모든 레퍼런스 파일 처리')
    parser.add_argument('--audio', type=str, help='처리할 특정 오디오 파일 경로')
    parser.add_argument('--midi', type=str, help='처리할 특정 MIDI 파일 경로')
    parser.add_argument('--song_id', type=str, help='UUID 기반 곡 ID (지정 시 title 무시)')
    parser.add_argument('--title', type=str, help='곡 제목 (song_id가 없을 때 제목으로 UUID 조회)')
    parser.add_argument('--description', type=str, help='곡 설명')
    parser.add_argument('--no-wait', action='store_true', help='작업 완료를 기다리지 않음')
    parser.add_argument('--list-songs', action='store_true', help='사용 가능한 모든 곡 정보 출력')
    
    args = parser.parse_args()
    
    if args.list_songs:
        # 사용 가능한 곡 목록 출력
        songs_data = get_songs_data()
        if songs_data:
            print("\n사용 가능한 곡 목록:")
            print("-" * 80)
            print(f"{'제목':<20} {'아티스트':<20} {'UUID':<36}")
            print("-" * 80)
            for song in songs_data:
                print(f"{song.get('title', ''):<20} {song.get('artist', ''):<20} {song.get('song_id', ''):<36}")
            print("-" * 80)
    elif args.all:
        # 모든 파일 처리
        process_all_files()
    elif args.audio:
        # 특정 파일만 처리
        process_reference_api(
            audio_path=args.audio,
            midi_path=args.midi,
            song_id=args.song_id,
            title=args.title,
            description=args.description,
            wait_for_result=not args.no_wait
        )
    else:
        parser.print_help()

if __name__ == "__main__":
    main() 