import os
import requests
import json
import time
from typing import Dict, Any, List, Optional

class GrokFeedbackGenerator:
    """
    GROK API를 사용하여 음악 연주 분석 결과에 대한 피드백을 생성하는 클래스
    """
    
    def __init__(self, api_key: Optional[str] = None, api_url: Optional[str] = None):
        """
        GrokFeedbackGenerator 초기화
        
        Args:
            api_key: GROK API 키 (없으면 환경 변수에서 가져옴)
            api_url: GROK API URL (없으면 기본값 사용)
        """
        self.api_key = api_key or os.environ.get('GROK_API_KEY')
        self.api_url = api_url or os.environ.get('GROK_API_URL', 'https://api.grok.ai/v1/chat/completions')
        
        if not self.api_key:
            raise ValueError("GROK API 키가 제공되지 않았습니다. 'GROK_API_KEY' 환경 변수를 설정하거나 초기화 시 제공하세요.")
    
    def create_prompt_from_analysis(self, analysis_data: Dict[str, Any]) -> str:
        """
        분석 데이터에서 프롬프트 생성
        
        Args:
            analysis_data: 오디오 분석 결과 데이터
            
        Returns:
            GROK API에 보낼 프롬프트
        """
        # 기본 정보 추출
        tempo = analysis_data.get('tempo', 0)
        number_of_notes = analysis_data.get('number_of_notes', 0)
        duration = analysis_data.get('duration', 0)
        techniques = analysis_data.get('techniques', [])
        
        # 사용된 기법 빈도 분석
        technique_counts = {}
        for technique_list in techniques:
            for tech in technique_list:
                if tech in technique_counts:
                    technique_counts[tech] += 1
                else:
                    technique_counts[tech] = 1
        
        # 프롬프트 구성
        prompt = f"""당신은 기타 연주 분석 전문가입니다. 연주자에게 도움이 되는 구체적인 피드백을 제공해주세요.

분석 데이터:
- 템포: {tempo} BPM
- 노트 수: {number_of_notes}개
- 연주 길이: {duration:.2f}초
- 사용된 기법: {technique_counts}

위 정보를 바탕으로 연주자에게 다음을 포함한 친절하고 전문적인 피드백을 제공해주세요:
1. 연주의 전반적인 평가
2. 템포와 리듬에 대한 피드백
3. 사용된 기법(테크닉)에 대한 평가와 조언
4. 다음 연습을 위한 구체적인 제안
5. 격려와 긍정적인 코멘트

피드백은 300자 내외로 작성해주세요.
"""
        return prompt
    
    def create_prompt_from_comparison(self, comparison_data: Dict[str, Any]) -> str:
        """
        비교 분석 데이터에서 프롬프트 생성
        
        Args:
            comparison_data: 오디오 비교 분석 결과 데이터
            
        Returns:
            GROK API에 보낼 프롬프트
        """
        # 점수 정보 추출
        scores = comparison_data.get('scores', {})
        overall_score = scores.get('overall_score', 0)
        tempo_match = scores.get('tempo_match_percentage', 0)
        pitch_match = scores.get('pitch_match_percentage', 0)
        rhythm_match = scores.get('rhythm_match_percentage', 0)
        technique_match = scores.get('technique_match_percentage', 0)
        expression_similarity = scores.get('expression_similarity', 0)
        
        # 사용자 연주 정보
        user_features = comparison_data.get('user_features', {})
        user_tempo = user_features.get('tempo', 0)
        user_techniques = user_features.get('techniques', [])
        
        # 참조 연주 정보
        ref_features = comparison_data.get('reference_features', {})
        ref_tempo = ref_features.get('tempo', 0)
        ref_techniques = ref_features.get('techniques', [])
        
        # 노트별 비교 정보
        note_comparisons = comparison_data.get('note_comparisons', [])
        timing_stats = {'better': 0, 'worse': 0, 'same': 0}
        if note_comparisons:
            for note in note_comparisons:
                comparison = note.get('timing_comparison', '')
                if comparison in timing_stats:
                    timing_stats[comparison] += 1
            
            total_notes = len(note_comparisons)
            better_percentage = (timing_stats['better'] / total_notes) * 100 if total_notes > 0 else 0
            worse_percentage = (timing_stats['worse'] / total_notes) * 100 if total_notes > 0 else 0
            same_percentage = (timing_stats['same'] / total_notes) * 100 if total_notes > 0 else 0
        
        # 사용된 기법 비교
        user_technique_counts = {}
        ref_technique_counts = {}
        
        for technique_list in user_techniques:
            for tech in technique_list:
                if tech in user_technique_counts:
                    user_technique_counts[tech] += 1
                else:
                    user_technique_counts[tech] = 1
                    
        for technique_list in ref_techniques:
            for tech in technique_list:
                if tech in ref_technique_counts:
                    ref_technique_counts[tech] += 1
                else:
                    ref_technique_counts[tech] = 1
        
        # 프롬프트 구성
        prompt = f"""당신은 전문 기타 선생님입니다. 학생의 연주를 모범 연주와 비교 분석한 결과를 바탕으로 구체적이고 도움이 되는 피드백을 제공해주세요.

비교 분석 결과:
- 종합 점수: {overall_score:.1f}점 (100점 만점)
- 템포 일치도: {tempo_match:.1f}% (학생: {user_tempo:.1f} BPM, 모범: {ref_tempo:.1f} BPM)
- 음정 일치도: {pitch_match:.1f}%
- 리듬 일치도: {rhythm_match:.1f}%
- 기법(테크닉) 일치도: {technique_match:.1f}%
"""

        if expression_similarity:
            prompt += f"- 표현력 유사도: {expression_similarity:.1f}%\n"
            
        if note_comparisons:
            prompt += f"""
노트별 타이밍 분석:
- 모범 연주보다 타이밍이 좋은 노트: {timing_stats['better']}개 ({better_percentage:.1f}%)
- 모범 연주보다 타이밍이 부족한 노트: {timing_stats['worse']}개 ({worse_percentage:.1f}%)
- 모범 연주와 타이밍이 동일한 노트: {timing_stats['same']}개 ({same_percentage:.1f}%)
"""

        prompt += f"""
학생이 사용한 기법: {user_technique_counts}
모범 연주에서 사용된 기법: {ref_technique_counts}

위 정보를 바탕으로 학생에게 다음을 포함한 구체적이고 건설적인 피드백을 제공해주세요:
1. 연주의 전반적인 평가와 강점
2. 템포와 리듬 유지에 대한 피드백
3. 음정 정확도에 대한 평가
4. 기법(테크닉) 사용에 대한 조언
5. 개선이 필요한 가장 중요한 부분 2-3가지
6. 향상을 위한 구체적인 연습 방법 제안
7. 격려와 긍정적인 마무리

피드백은 500자 내외로 작성해주세요. 기술적인 용어는 적절히 사용하되, 초보자도 이해할 수 있는 설명을 포함해주세요.
"""
        return prompt
    
    def generate_feedback(self, data: Dict[str, Any], is_comparison: bool = False, max_retries: int = 3) -> Dict[str, Any]:
        """
        분석 데이터를 기반으로 GROK API를 사용하여 피드백 생성
        
        Args:
            data: 분석 데이터 또는 비교 데이터
            is_comparison: 비교 데이터인지 여부
            max_retries: 최대 재시도 횟수
            
        Returns:
            생성된 피드백과 메타데이터를 포함한 딕셔너리
        """
        # 분석 타입에 따라 적절한 프롬프트 생성
        if is_comparison:
            prompt = self.create_prompt_from_comparison(data)
        else:
            prompt = self.create_prompt_from_analysis(data)
        
        # API 요청 헤더
        headers = {
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.api_key}"
        }
        
        # API 요청 데이터
        request_data = {
            "model": "grok-1",
            "messages": [
                {"role": "system", "content": "당신은 전문적인 기타 선생님으로서 연주를 분석하고 친절하고 건설적인 피드백을 제공합니다."},
                {"role": "user", "content": prompt}
            ],
            "temperature": 0.7,
            "max_tokens": 800
        }
        
        # 재시도 로직
        for attempt in range(max_retries):
            try:
                response = requests.post(
                    self.api_url,
                    headers=headers,
                    json=request_data,
                    timeout=30  # 30초 타임아웃
                )
                
                if response.status_code == 200:
                    response_data = response.json()
                    feedback_text = response_data['choices'][0]['message']['content']
                    
                    return {
                        "feedback": feedback_text,
                        "metadata": {
                            "model": "grok-1",
                            "timestamp": time.time(),
                            "prompt_tokens": response_data.get('usage', {}).get('prompt_tokens', 0),
                            "completion_tokens": response_data.get('usage', {}).get('completion_tokens', 0),
                            "total_tokens": response_data.get('usage', {}).get('total_tokens', 0)
                        }
                    }
                elif response.status_code == 429:  # 속도 제한
                    wait_time = min(2 ** attempt, 60)  # 지수 백오프
                    time.sleep(wait_time)
                    continue
                else:
                    error_message = f"GROK API 오류: {response.status_code} - {response.text}"
                    return {"error": error_message}
                
            except Exception as e:
                if attempt < max_retries - 1:
                    wait_time = min(2 ** attempt, 60)
                    time.sleep(wait_time)
                else:
                    return {"error": f"피드백 생성 중 오류 발생: {str(e)}"}
        
        return {"error": "최대 재시도 횟수를 초과했습니다."}


# 간단한 사용 예시
if __name__ == "__main__":
    # 환경 변수에서 API 키를 가져오거나 직접 지정
    api_key = os.environ.get('GROK_API_KEY', 'your_api_key_here')
    
    # 피드백 생성기 초기화
    feedback_generator = GrokFeedbackGenerator(api_key=api_key)
    
    # 테스트 데이터
    test_data = {
        "tempo": 120.5,
        "number_of_notes": 42,
        "duration": 60.3,
        "techniques": [
            ["bend"], ["hammer"], ["normal"], ["pull"], ["slide"]
        ]
    }
    
    # 피드백 생성
    feedback = feedback_generator.generate_feedback(test_data)
    print(feedback.get('feedback', feedback.get('error')))