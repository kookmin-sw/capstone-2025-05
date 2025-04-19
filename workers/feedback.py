import os
import json
import requests
import time
from typing import Dict, Any, List, Optional
import logging

# 로거 설정
logger = logging.getLogger(__name__)

class GrokFeedbackGenerator:
    """
    GROK API를 사용하여 음악 분석 결과에 대한 텍스트 피드백을 생성하는 클래스
    """
    def __init__(self):
        """GROK API와의 통신을 위한 설정을 초기화합니다."""
        self.api_key = os.environ.get('GROK_API_KEY')
        self.api_url = os.environ.get('GROK_API_URL', 'https://api.grok.ai/v1/chat/completions')
        self.max_retries = 3
        self.retry_delay = 2  # 초 단위
        
        if not self.api_key:
            logger.warning("GROK_API_KEY가 설정되지 않았습니다. 피드백 생성이 불가능합니다.")

    def generate_feedback(self, analysis_result: Dict[str, Any], is_comparison: bool = False) -> Dict[str, Any]:
        """
        분석 결과에 기반한 피드백을 생성합니다.
        
        Args:
            analysis_result: 분석 또는 비교 결과 데이터 
            is_comparison: 비교 분석 결과인지 여부
            
        Returns:
            생성된 피드백 텍스트와 메타데이터를 포함하는 딕셔너리
        """
        if not self.api_key:
            return {"error": "GROK API 키가 설정되지 않았습니다."}
        
        try:
            # 분석 유형에 따라 적절한 프롬프트 생성
            if is_comparison:
                prompt = self._create_comparison_prompt(analysis_result)
            else:
                prompt = self._create_analysis_prompt(analysis_result)
            
            # GROK API 호출
            for attempt in range(self.max_retries):
                try:
                    response = self._call_grok_api(prompt)
                    
                    # 성공적인 응답 처리
                    if response and 'choices' in response:
                        feedback_text = response['choices'][0]['message']['content']
                        
                        return {
                            "feedback": feedback_text,
                            "metadata": {
                                "model": response.get('model', 'unknown'),
                                "tokens": {
                                    "prompt_tokens": response.get('usage', {}).get('prompt_tokens', 0),
                                    "completion_tokens": response.get('usage', {}).get('completion_tokens', 0),
                                    "total_tokens": response.get('usage', {}).get('total_tokens', 0)
                                }
                            }
                        }
                    else:
                        # 유효한 응답이 없는 경우
                        logger.error(f"유효하지 않은 API 응답: {response}")
                        
                except Exception as e:
                    logger.error(f"API 호출 중 오류 발생 (시도 {attempt+1}/{self.max_retries}): {str(e)}")
                    if attempt < self.max_retries - 1:
                        time.sleep(self.retry_delay * (2 ** attempt))  # 지수 백오프
                    else:
                        raise
            
            # 최대 재시도 후에도 성공하지 못한 경우
            return {"error": "최대 재시도 횟수를 초과했습니다."}
            
        except Exception as e:
            logger.exception(f"피드백 생성 중 오류 발생: {str(e)}")
            return {"error": f"피드백 생성 중 오류 발생: {str(e)}"}

    def _call_grok_api(self, prompt: str) -> Dict[str, Any]:
        """
        GROK API를 호출하여 피드백을 생성합니다.
        
        Args:
            prompt: GROK API에 전송할 프롬프트
            
        Returns:
            API 응답 데이터
        """
        headers = {
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.api_key}"
        }
        
        data = {
            "model": "grok-3-mini-fast",
            "messages": [
                {"role": "system", "content": "당신은 전문적인 음악 교육자입니다. 학생들에게 친절하고 유용한 음악 연주 피드백을 제공합니다."},
                {"role": "user", "content": prompt}
            ],
            "temperature": 0.7,
            "max_tokens": 2000
        }
        
        response = requests.post(self.api_url, headers=headers, json=data)
        
        if response.status_code == 200:
            return response.json()
        else:
            logger.error(f"API 오류: {response.status_code} - {response.text}")
            raise Exception(f"API 호출 실패: {response.status_code} - {response.text}")

    def _create_analysis_prompt(self, analysis_result: Dict[str, Any]) -> str:
        """
        단일 오디오 분석 결과에 대한 프롬프트를 생성합니다.
        
        Args:
            analysis_result: 분석 결과 데이터
            
        Returns:
            생성된 프롬프트 문자열
        """
        # 주요 분석 데이터 추출
        tempo = analysis_result.get('tempo', 0)
        num_notes = analysis_result.get('number_of_notes', 0)
        duration = analysis_result.get('duration', 0)
        techniques = analysis_result.get('techniques', [])
        
        # 사용된 기술 집계
        technique_counts = {}
        for tech_list in techniques:
            for tech in tech_list:
                technique_counts[tech] = technique_counts.get(tech, 0) + 1
        
        # 프롬프트 구성
        prompt = f"""
다음은 학생의 기타 연주에 대한 분석 데이터입니다:

- 템포: {tempo} BPM
- 노트 수: {num_notes}개
- 연주 길이: {duration:.2f}초

사용된 기법:
"""
        
        for tech, count in technique_counts.items():
            percentage = (count / max(1, len(techniques))) * 100
            prompt += f"- {tech}: {count}회 ({percentage:.1f}%)\n"
        
        prompt += """
위 분석 데이터를 바탕으로 학생에게 도움이 될 수 있는 연주 피드백을 제공해주세요. 
피드백은 다음 내용을 포함해야 합니다:

1. 연주의 전반적인 평가
2. 템포와 리듬에 대한 피드백
3. 사용된 기법에 대한 평가와 조언
4. 향후 연습을 위한 제안
5. 격려와 긍정적인 피드백

한국어로 친절하고 교육적인 톤으로 작성해주세요. 부정적인 피드백보다는 건설적인 조언에 중점을 두세요.
"""
        
        return prompt

    def _create_comparison_prompt(self, comparison_result: Dict[str, Any]) -> str:
        """
        비교 분석 결과에 대한 프롬프트를 생성합니다.
        
        Args:
            comparison_result: 비교 분석 결과 데이터
            
        Returns:
            생성된 프롬프트 문자열
        """
        # 비교 데이터 추출 시도
        try:
            scores = comparison_result.get('scores', {})
            
            tempo_match = scores.get('tempo_match_percentage', 0)
            pitch_match = scores.get('pitch_match_percentage', 0)
            rhythm_match = scores.get('rhythm_match_percentage', 0)
            technique_match = scores.get('technique_match_percentage', 0)
            overall_score = scores.get('overall_score', 0)
            expression_similarity = scores.get('expression_similarity', None)
            
            # 연주 기법 데이터 추출
            user_features = comparison_result.get('user_features', {})
            ref_features = comparison_result.get('reference_features', {})
            
            user_techniques = user_features.get('techniques', [])
            ref_techniques = ref_features.get('techniques', [])
            
            # 노트별 비교 데이터 추출
            note_comparisons = comparison_result.get('note_comparisons', [])
            
            # 노트 정확도 통계 계산
            better_notes = 0
            same_notes = 0
            worse_notes = 0
            
            for note in note_comparisons:
                if note.get('timing_comparison') == 'better':
                    better_notes += 1
                elif note.get('timing_comparison') == 'same':
                    same_notes += 1
                else:
                    worse_notes += 1
            
            total_notes = max(1, len(note_comparisons))
            better_percentage = (better_notes / total_notes) * 100
            same_percentage = (same_notes / total_notes) * 100
            worse_percentage = (worse_notes / total_notes) * 100
            
            # 사용된 기법 집계
            user_technique_counts = {}
            for tech_list in user_techniques:
                for tech in tech_list:
                    user_technique_counts[tech] = user_technique_counts.get(tech, 0) + 1
            
            ref_technique_counts = {}
            for tech_list in ref_techniques:
                for tech in tech_list:
                    ref_technique_counts[tech] = ref_technique_counts.get(tech, 0) + 1
            
            # 프롬프트 구성
            prompt = f"""
다음은 학생의 기타 연주와 모범 연주 간의 비교 분석 데이터입니다:

종합 점수: {overall_score:.2f}%

세부 점수:
- 템포 일치도: {tempo_match:.2f}%
- 음정 일치도: {pitch_match:.2f}%
- 리듬 일치도: {rhythm_match:.2f}%
- 기법 일치도: {technique_match:.2f}%
"""
            
            if expression_similarity is not None:
                prompt += f"- 표현력 유사도: {expression_similarity:.2f}%\n"
            
            prompt += f"""
노트별 타이밍 분석:
- 모범 연주보다 정확한 노트: {better_notes}개 ({better_percentage:.1f}%)
- 모범 연주와 동일한 노트: {same_notes}개 ({same_percentage:.1f}%)
- 모범 연주보다 부정확한 노트: {worse_notes}개 ({worse_percentage:.1f}%)

학생이 사용한 기법:
"""
            
            for tech, count in user_technique_counts.items():
                percentage = (count / max(1, len(user_techniques))) * 100
                prompt += f"- {tech}: {count}회 ({percentage:.1f}%)\n"
            
            prompt += "\n모범 연주에서 사용된 기법:\n"
            
            for tech, count in ref_technique_counts.items():
                percentage = (count / max(1, len(ref_techniques))) * 100
                prompt += f"- {tech}: {count}회 ({percentage:.1f}%)\n"
            
        except Exception as e:
            logger.error(f"비교 결과 데이터 처리 중 오류: {str(e)}")
            # 오류 발생 시 간소화된 프롬프트 사용
            prompt = f"""
다음은 학생의 기타 연주와 모범 연주 간의 비교 분석 데이터입니다:

종합 점수: {comparison_result.get('scores', {}).get('overall_score', 0):.2f}%
"""
        
        prompt += """
위 비교 분석 데이터를 바탕으로 학생에게 도움이 될 수 있는 연주 피드백을 제공해주세요. 
피드백은 다음 내용을 포함해야 합니다:

1. 전반적인 평가와 강점
2. 템포, 리듬, 음정 정확도에 대한 피드백
3. 기법 사용에 대한 조언
4. 개선이 필요한 부분과 연습 방법 제안
5. 격려와 긍정적인 마무리

한국어로 친절하고 교육적인 톤으로 작성해주세요. 부정적인 피드백보다는 건설적인 조언에 중점을 두세요.
"""
        
        return prompt