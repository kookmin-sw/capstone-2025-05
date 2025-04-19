import requests
import time
import json
from datetime import datetime

# 서버 URL
API_URL = "https://analysis.maple.ne.kr"

# 1. 분석 API 요청
def test_analyze(generate_feedback=True):
    print("오디오 분석 테스트 시작...")
    
    with open("/home/lovel/develop/maple-audio-analyzer/test/ref/homecoming.wav", "rb") as f:
        files = {"file": f}
        params = {
            "user_id": "test-user", 
            "song_id": "test-song",
            "generate_feedback": str(generate_feedback).lower()  # GROK 피드백 생성 옵션 추가
        }
        
        response = requests.post(f"{API_URL}/api/v1/analyze", files=files, params=params)
        print(f"응답 상태 코드: {response.status_code}")
        
        if response.status_code == 200:
            result = response.json()
            print(f"작업 ID: {result.get('task_id')}")
            print(f"GROK 피드백 생성 옵션: {'활성화' if generate_feedback else '비활성화'}")
            return result.get('task_id')
        else:
            print(f"오류: {response.text}")
            return None

# 2. 비교 API 요청
def test_compare(generate_feedback=True):
    print("\n오디오 비교 테스트 시작...")
    
    with open("/home/lovel/develop/maple-audio-analyzer/test/error/homecoming-error-1.wav", "rb") as user_file, \
         open("/home/lovel/develop/maple-audio-analyzer/test/ref/homecoming.wav", "rb") as ref_file, \
         open("/home/lovel/develop/maple-audio-analyzer/test/midi/homecoming.mid", "rb") as midi_file:
        
        files = {
            "user_file": user_file,
            "reference_file": ref_file,
            "midi_file": midi_file
        }
        params = {
            "user_id": "test-user", 
            "song_id": "test-song",
            "generate_feedback": str(generate_feedback).lower()  # GROK 피드백 생성 옵션 추가
        }
        
        response = requests.post(f"{API_URL}/api/v1/compare", files=files, params=params)
        print(f"응답 상태 코드: {response.status_code}")
        
        if response.status_code == 200:
            result = response.json()
            print(f"작업 ID: {result.get('task_id')}")
            print(f"GROK 피드백 생성 옵션: {'활성화' if generate_feedback else '비활성화'}")
            return result.get('task_id')
        else:
            print(f"오류: {response.text}")
            return None

# 3. 작업 상태 확인 (개선된 버전)
def check_task_status(task_id, max_attempts=30, wait_time=3):
    """
    작업 상태를 확인하고 결과를 기다립니다.
    
    Args:
        task_id: 확인할 작업 ID
        max_attempts: 최대 시도 횟수 (기본값: 30회 - 최대 90초)
        wait_time: 각 시도 사이의 대기 시간(초) (기본값: 3초)
    """
    print(f"\n작업 상태 확인 중... (ID: {task_id})")
    print(f"최대 {max_attempts}회 시도, 각 시도 사이 {wait_time}초 대기 (최대 {max_attempts * wait_time}초)")
    
    last_status = None
    last_progress = None
    idle_count = 0  # 상태 변경이 없는 시도 횟수를 추적
    last_update_time = time.time()
    
    for i in range(max_attempts):
        try:
            response = requests.get(f"{API_URL}/api/v1/tasks/{task_id}")
            current_time = time.time()
            time_diff = current_time - last_update_time
            
            if response.status_code == 200:
                status = response.json()
                current_status = status.get('status')
                current_progress = status.get('progress', 0)
                
                # 항상 현재 시도 횟수 출력
                print(f"시도 {i+1}/{max_attempts} - 상태: {current_status}, 진행률: {current_progress}%", end="")
                
                # 상태나 진행률이 변경된 경우
                if current_status != last_status or current_progress != last_progress:
                    print(f" - 업데이트됨", flush=True)
                    last_status = current_status
                    last_progress = current_progress
                    idle_count = 0  # 변경이 감지되었으므로 idle_count 초기화
                    last_update_time = current_time
                else:
                    idle_count += 1
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
                        
                        # GROK 피드백 확인 (추가된 부분)
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
                        
                        # 피드백만 따로 텍스트 파일로 저장 (추가된 부분)
                        if 'feedback' in status.get('result', {}):
                            feedback_filename = f"feedback_{task_id[:8]}.txt"
                            with open(feedback_filename, 'w') as f:
                                f.write(status['result']['feedback'])
                            print(f"GROK 피드백이 {feedback_filename}에 저장되었습니다.")
                        
                    elif status.get('error'):
                        print(f"\n오류: {status.get('error')}")
                    return status
            else:
                print(f"오류: {response.text}")
            
        except Exception as e:
            print(f"요청 중 예외 발생: {e}")
        
        # 마지막 시도가 아니면 기다림
        if i < max_attempts - 1:
            time.sleep(wait_time)
    
    print(f"\n최대 시도 횟수({max_attempts}회)를 초과했습니다. 작업이 여전히 진행 중일 수 있습니다.")
    print(f"나중에 다음 URL을 직접 확인해보세요: {API_URL}/api/v1/tasks/{task_id}")
    return None

# 메인 테스트 실행
def run_tests(generate_feedback=True):
    # 실행 옵션 출력
    print(f"테스트 실행 - GROK 피드백 생성: {'활성화' if generate_feedback else '비활성화'}")
    
    # 분석 테스트
    task_id = test_analyze(generate_feedback)
    if task_id:
        analysis_result = check_task_status(task_id)
        
        # 결과 파일 생성 (analysis.json)
        if analysis_result and analysis_result.get('result'):
            print("\nanalysis.json 파일 생성 중...")
            formatted_result = {
                "status": "success",
                "analysis": analysis_result.get('result')
            }
            with open('analysis.json', 'w') as f:
                json.dump(formatted_result, f, indent=2)
            print("analysis.json 파일이 생성되었습니다.")
    
    # 잠시 대기
    time.sleep(2)
    
    # 비교 테스트
    task_id = test_compare(generate_feedback)
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
    # GROK 피드백 생성 옵션을 True로 설정하여 테스트 실행
    run_tests(generate_feedback=True)