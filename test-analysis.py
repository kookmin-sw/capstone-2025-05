import requests
import time
import json
import os
from pathlib import Path
from datetime import datetime

# 서버 URL
API_URL = "http://localhost:8000"
# API_URL = "https://analysis.maple.ne.kr"
# SONGS_API_URL = "http://localhost:8000/api/v1/songs"
SONGS_API_URL = "https://media.maple.ne.kr/api/v1/songs"
# 오디오 파일 디렉터리
USER_DIR = Path("test/user")  # 테스트 사용자 오디오 파일 디렉토리
REF_DIR = Path("test/ref")

def get_songs_data():
    """
    songs API에서 곡 목록을 가져옵니다.
    """
    try:
        response = requests.get(SONGS_API_URL)
        if response.status_code == 200:
            return response.json()
        else:
            print(f"곡 정보 API 호출 실패: {response.status_code} - {response.text}")
            return None
    except Exception as e:
        print(f"곡 정보 API 호출 중 오류 발생: {str(e)}")
        return None

def display_available_songs():
    """
    songs API에서 가져온 곡 목록과 로컬 파일을 매칭하여 표시합니다.
    매칭된 파일만 번호를 매겨 표시합니다.
    """
    songs_data = get_songs_data()
    if not songs_data:
        print("곡 정보를 가져올 수 없습니다.")
        return None, None
    
    # 파일명과 UUID 매핑 생성 (소문자로 통일하여 비교)
    title_to_uuid_map = {}
    title_to_original = {}  # 원래 대소문자를 보존하기 위한 매핑
    
    for song in songs_data:
        title = song.get('title', '')
        title_lower = title.lower()
        title_to_uuid_map[title_lower] = song.get('song_id')
        title_to_original[title_lower] = title
    
    # 로컬에 존재하는 사용자 오디오 파일 목록 가져오기
    if not USER_DIR.exists():
        print(f"사용자 오디오 디렉토리가 존재하지 않습니다: {USER_DIR}")
        return None, None
    
    user_files = [f for f in USER_DIR.glob('*.wav')]
    
    # 매칭된 파일 찾기
    matched_files = []
    unmatched_files = []
    
    for user_file in user_files:
        # 파일 이름에서 '-user-' 앞부분을 제목으로 추출 (예: homecoming-user-1.wav -> homecoming)
        file_name = user_file.stem
        title_parts = file_name.split('-user')
        file_title = title_parts[0].lower() if title_parts else file_name.lower()
        
        # API 목록에서 제목과 일치하는 항목 찾기
        if file_title in title_to_uuid_map:
            uuid = title_to_uuid_map[file_title]
            original_title = title_to_original[file_title]
            matched_files.append((user_file, original_title, uuid))
        else:
            unmatched_files.append(user_file)
    
    # 매칭된 파일이 없으면 종료
    if not matched_files:
        print("매칭된 파일이 없습니다.")
        return None, None
    
    # 매칭된 파일만 리스트로 출력
    print("\n사용 가능한 사용자 오디오 파일:")
    print("-" * 80)
    print(f"{'#':<4} {'사용자 파일명':<30} {'제목(API)':<20} {'UUID':<36}")
    print("-" * 80)
    
    for i, (user_file, original_title, uuid) in enumerate(matched_files, 1):
        print(f"{i:<4} {user_file.name:<30} {original_title:<20} {uuid:<36}")
    
    print("-" * 80)
    print(f"매칭된 파일: {len(matched_files)}개")
    if unmatched_files:
        print(f"매칭되지 않은 파일: {len(unmatched_files)}개")
    print("-" * 80)
    
    return matched_files, songs_data

def select_file_for_analysis():
    """
    매칭된 파일 중에서 분석할 파일을 선택합니다.
    """
    matched_files, songs_data = display_available_songs()
    if not matched_files:
        return None, None, None
    
    while True:
        try:
            selection = input("\n분석할 파일 번호를 입력하세요 (0: 취소): ")
            if selection == "0":
                print("취소되었습니다.")
                return None, None, None
            
            index = int(selection) - 1
            if 0 <= index < len(matched_files):
                selected_file, title, uuid = matched_files[index]
                return selected_file, title, uuid
            else:
                print(f"유효한 번호를 입력하세요 (1-{len(matched_files)})")
        except ValueError:
            print("숫자를 입력하세요.")

# 1. 분석 API 요청
def test_analyze(audio_file, song_id, user_id="test-user", generate_feedback=True):
    print("\n오디오 분석 테스트 시작...")
    print(f"파일: {audio_file}")
    print(f"곡 ID: {song_id}")
    print(f"사용자 ID: {user_id}")
    print(f"GROK 피드백: 활성화")
    
    with open(audio_file, "rb") as f:
        files = {"file": f}
        data = {
            "user_id": user_id, 
            "song_id": song_id,
            "generate_feedback": "true"  # 항상 GROK 피드백 활성화
        }
        
        print("API 요청 전송 중...")
        response = requests.post(f"{API_URL}/api/v1/analyze", files=files, data=data)
        print(f"응답 상태 코드: {response.status_code}")
        
        if response.status_code == 200:
            result = response.json()
            print(f"작업 ID: {result.get('task_id')}")
            return result.get('task_id')
        else:
            print(f"오류: {response.text}")
            return None

# 2. 비교 API 요청
def test_compare(audio_file, song_id, user_id="test-user", generate_feedback=True):
    print("\n오디오 비교 테스트 시작...")
    print(f"파일: {audio_file}")
    print(f"곡 ID: {song_id}")
    print(f"사용자 ID: {user_id}")
    print(f"GROK 피드백: 활성화")
    
    with open(audio_file, "rb") as user_file:
        files = {
            "user_file": user_file,
        }
        data = {
            "user_id": user_id, 
            "song_id": song_id,
            "generate_feedback": "true"  # 항상 GROK 피드백 활성화
        }
        
        print("API 요청 전송 중...")
        response = requests.post(f"{API_URL}/api/v1/compare-with-reference", files=files, data=data)
        print(f"응답 상태 코드: {response.status_code}")
        
        if response.status_code == 200:
            result = response.json()
            print(f"작업 ID: {result.get('task_id')}")
            return result.get('task_id')
        else:
            print(f"오류: {response.text}")
            return None

# 3. 작업 상태 확인 (무한 대기 버전)
def check_task_status(task_id, wait_time=3):
    """
    작업 상태를 확인하고 결과를 기다립니다.
    작업이 완료될 때까지 무한정 대기합니다 (Ctrl+C로 중단 가능).
    
    Args:
        task_id: 확인할 작업 ID
        wait_time: 각 시도 사이의 대기 시간(초) (기본값: 3초)
    """
    print(f"\n작업 상태 확인 중... (ID: {task_id})")
    print(f"작업이 완료될 때까지 계속 확인합니다. 취소하려면 Ctrl+C를 누르세요.")
    
    last_status = None
    last_progress = None
    last_update_time = time.time()
    attempt_count = 0
    
    try:
        while True:
            attempt_count += 1
            
            try:
                response = requests.get(f"{API_URL}/api/v1/tasks/{task_id}")
                current_time = time.time()
                time_diff = current_time - last_update_time
                
                if response.status_code == 200:
                    status = response.json()
                    current_status = status.get('status')
                    current_progress = status.get('progress', 0)
                    
                    # 항상 현재 시도 횟수 출력
                    print(f"시도 {attempt_count} - 상태: {current_status}, 진행률: {current_progress}%", end="")
                    
                    # 상태나 진행률이 변경된 경우
                    if current_status != last_status or current_progress != last_progress:
                        print(f" - 업데이트됨", flush=True)
                        last_status = current_status
                        last_progress = current_progress
                        last_update_time = current_time
                    else:
                        # 일정 시간(15초) 이상 변화가 없으면 작업이 계속 진행 중임을 표시
                        if time_diff > 15:
                            print(f" - 작업 진행 중 ({time_diff:.1f}초 동안 상태 변화 없음)", flush=True)
                            last_update_time = current_time
                        else:
                            print("", flush=True)  # 줄바꿈만 처리
                    
                    if current_status in ['SUCCESS', 'FAILURE']:
                        if status.get('result'):
                            print("\n결과 수신 완료!")
                            # 전체 결과 출력은 너무 길 수 있으므로 주요 부분만 출력
                            if 'scores' in status.get('result', {}):
                                scores = status['result']['scores']
                                print("\n점수 요약:")
                                for key, value in scores.items():
                                    print(f"  - {key}: {value}")
                            else:
                                print("\n요약 결과:")
                                summary = {k: v for k, v in status.get('result', {}).items() 
                                          if k in ['tempo', 'number_of_notes', 'duration']}
                                print(json.dumps(summary, indent=2))
                            
                            # GROK 피드백 확인
                            if 'feedback' in status.get('result', {}):
                                print("\n----- GROK 피드백 -----")
                                print(status['result']['feedback'])
                                print("-----------------------")
                            elif 'feedback_error' in status.get('result', {}):
                                print("\n----- GROK 피드백 오류 -----")
                                print(status['result']['feedback_error'])
                                print("---------------------------")
                                
                            # 전체 결과를 파일로 저장
                            filename = f"result_{task_id[:8]}.json"
                            with open(filename, 'w') as f:
                                json.dump(status.get('result'), f, indent=2)
                            print(f"\n전체 결과가 {filename}에 저장되었습니다.")
                            
                            # 피드백만 따로 텍스트 파일로 저장
                            if 'feedback' in status.get('result', {}):
                                feedback_filename = f"feedback_{task_id[:8]}.txt"
                                with open(feedback_filename, 'w') as f:
                                    f.write(status['result']['feedback'])
                                print(f"GROK 피드백이 {feedback_filename}에 저장되었습니다.")
                            
                        elif status.get('error'):
                            print(f"\n오류: {status.get('error')}")
                        
                        return status
                else:
                    print(f"오류: {response.status_code} - {response.text}")
                
            except Exception as e:
                print(f"요청 중 예외 발생: {e}")
            
            # 일정 시간 대기 후 다시 시도
            time.sleep(wait_time)
    
    except KeyboardInterrupt:
        # 사용자가 Ctrl+C로 중단한 경우
        print("\n\n사용자에 의해 작업 확인이 중단되었습니다.")
        print(f"나중에 다음 URL을 직접 확인해보세요: {API_URL}/api/v1/tasks/{task_id}")
        return None

# 메인 메뉴 표시
def show_main_menu():
    print("\n=== 오디오 분석 테스트 도구 ===")
    print("1. 오디오 분석 (단순 분석)")
    print("2. 오디오 비교 (레퍼런스와 비교)")
    print("3. 종료")
    choice = input("선택하세요 (1-3): ")
    return choice

# 메인 테스트 실행
def run_interactive_tests():
    while True:
        choice = show_main_menu()
        
        if choice == "3":
            print("프로그램을 종료합니다.")
            break
        
        # 파일 선택
        audio_file, title, song_id = select_file_for_analysis()
        if not audio_file:
            continue
        
        # 사용자 ID 입력 (선택 사항)
        user_id = input("사용자 ID를 입력하세요 (기본값: test-user): ").strip()
        if not user_id:
            user_id = "test-user"
        
        if choice == "1":
            # 분석 실행
            task_id = test_analyze(audio_file, song_id, user_id)
            if task_id:
                result = check_task_status(task_id)
                # 결과 파일 생성
                if result and result.get('result'):
                    print("\nanalysis.json 파일 생성 중...")
                    formatted_result = {
                        "status": "success",
                        "analysis": result.get('result')
                    }
                    with open('analysis.json', 'w') as f:
                        json.dump(formatted_result, f, indent=2)
                    print("analysis.json 파일이 생성되었습니다.")
        
        elif choice == "2":
            # 비교 실행
            task_id = test_compare(audio_file, song_id, user_id)
            if task_id:
                result = check_task_status(task_id)
                # 결과 파일 생성
                if result and result.get('result'):
                    print("\ncompare.json 파일 생성 중...")
                    formatted_result = {
                        "status": "success",
                        "comparison": result.get('result')
                    }
                    with open('compare.json', 'w') as f:
                        json.dump(formatted_result, f, indent=2)
                    print("compare.json 파일이 생성되었습니다.")

# 기존 스크립트 방식의 테스트 실행
def run_batch_tests():
    # 실행 옵션 출력
    print(f"배치 테스트 실행 - GROK 피드백 생성: 활성화")
    
    # # 분석 테스트
    # task_id = test_analyze(str(USER_DIR / "homecoming-user-1.wav"), "homecoming")
    # if task_id:
    #     analysis_result = check_task_status(task_id)
    #     # 결과 파일 생성 (analysis.json)
    #     if analysis_result and analysis_result.get('result'):
    #         print("\nanalysis.json 파일 생성 중...")
    #         formatted_result = {
    #             "status": "success",
    #             "analysis": analysis_result.get('result')
    #         }
    #         with open('analysis.json', 'w') as f:
    #             json.dump(formatted_result, f, indent=2)
    #         print("analysis.json 파일이 생성되었습니다.")
    
    # 비교 테스트
    task_id = test_compare(str(USER_DIR / "homecoming-user-1.wav"), "homecoming")
    if task_id:
        compare_result = check_task_status(task_id)
        # 결과 파일 생성 (compare.json)
        if compare_result and compare_result.get('result'):
            print("\ncompare.json 파일 생성 중...")
            formatted_result = {
                "status": "success",
                "comparison": compare_result.get('result')
            }
            with open('compare.json', 'w') as f:
                json.dump(formatted_result, f, indent=2)
            print("compare.json 파일이 생성되었습니다.")

if __name__ == "__main__":
    # 인터랙티브 모드로 실행
    run_interactive_tests()
    
    # 배치 모드로 실행 (기존 방식)
    # run_batch_tests()