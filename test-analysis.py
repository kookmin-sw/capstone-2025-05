import requests
import time
import json

# 1. 테스트할 API 엔드포인트 지정
ANALYZE_URL = 'http://localhost:8000/api/v1/analyze'
COMPARE_URL = 'http://localhost:8000/api/v1/compare'
TASK_STATUS_URL = 'http://localhost:8000/api/v1/tasks/{}'

# 2. 단일 오디오 파일 분석 테스트
print("\n======= 오디오 파일 분석 테스트 =======")
with open('test/ref/homecoming.wav', 'rb') as f:
    files = {'file': f}
    params = {'user_id': 'test-user', 'song_id': 'homecoming-test'}
    
    try:
        # 분석 요청 전송
        response = requests.post(ANALYZE_URL, files=files, params=params)
        response.raise_for_status()
        result = response.json()
        print(f"분석 요청 완료: {result}")
        
        # 작업 상태 확인 (최대 10초 동안 2초 간격으로)
        task_id = result['task_id']
        for i in range(5):
            try:
                status_resp = requests.get(TASK_STATUS_URL.format(task_id))
                if status_resp.status_code == 200:
                    status = status_resp.json()
                    print(f"작업 상태 ({i+1}): {status.get('status', 'Unknown')}")
                    if status.get('status') in ['SUCCESS', 'FAILURE']:
                        if status.get('result'):
                            print(f"분석 결과: {json.dumps(status['result'], indent=2)}")
                        elif status.get('error'):
                            print(f"분석 오류: {status['error']}")
                        break
                else:
                    print(f"상태 확인 실패 ({i+1}): {status_resp.text}")
            except Exception as e:
                print(f"상태 확인 중 오류 ({i+1}): {e}")
            
            time.sleep(2)
    except Exception as e:
        print(f"분석 요청 중 오류: {e}")

# 3. 오디오 파일 비교 테스트
print("\n======= 오디오 파일 비교 테스트 =======")
with open('test/error/homecoming-error-1.wav', 'rb') as user_file, \
     open('test/ref/homecoming.wav', 'rb') as ref_file, \
     open('test/midi/homecoming.mid', 'rb') as midi_file:
    
    files = {
        'user_file': user_file,
        'reference_file': ref_file,
        'midi_file': midi_file
    }
    params = {'user_id': 'test-user', 'song_id': 'homecoming-compare-test'}
    
    try:
        # 비교 요청 전송
        response = requests.post(COMPARE_URL, files=files, params=params)
        response.raise_for_status()
        result = response.json()
        print(f"비교 요청 완료: {result}")
        
        # 작업 상태 확인 (최대 10초 동안 2초 간격으로)
        task_id = result['task_id']
        for i in range(5):
            try:
                status_resp = requests.get(TASK_STATUS_URL.format(task_id))
                if status_resp.status_code == 200:
                    status = status_resp.json()
                    print(f"작업 상태 ({i+1}): {status.get('status', 'Unknown')}")
                    if status.get('status') in ['SUCCESS', 'FAILURE']:
                        if status.get('result'):
                            print(f"비교 결과 요약: ")
                            scores = status['result'].get('scores', {})
                            for key, value in scores.items():
                                print(f"  - {key}: {value}")
                        elif status.get('error'):
                            print(f"비교 오류: {status['error']}")
                        break
                else:
                    print(f"상태 확인 실패 ({i+1}): {status_resp.text}")
            except Exception as e:
                print(f"상태 확인 중 오류 ({i+1}): {e}")
            
            time.sleep(2)
    except Exception as e:
        print(f"비교 요청 중 오류: {e}")