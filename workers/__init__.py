# workers package

# SciPy 호환성 패치
# 최신 버전의 SciPy에서는 scipy.signal.hann이 scipy.signal.windows.hann으로 이동했습니다.
import scipy.signal

# 패치 적용: scipy.signal.hann이 없으면 windows.hann을 사용
if not hasattr(scipy.signal, 'hann') and hasattr(scipy.signal, 'windows') and hasattr(scipy.signal.windows, 'hann'):
    print("패치 적용: scipy.signal.hann -> scipy.signal.windows.hann")
    scipy.signal.hann = scipy.signal.windows.hann