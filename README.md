# 🎸 <span style="color:#A57865"><strong>AI 기반 기타 연주 분석 플랫폼</strong></span> - [MAPLE](https://maple.ne.kr/)

<div align="center"> 
  <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/logo.svg" width="70%" alt="MAPLE Logo"/>
</div>

<br>
<br>

## 📌 목차

1️⃣ [✨ 프로젝트 소개](#-프로젝트-소개)  
2️⃣ [🎯 프로젝트 추진 배경 및 목적](#-프로젝트-추진-배경-및-목적)  
3️⃣ [🦾 주요 기능](#-주요-기능)  
4️⃣ [🎬 소개 영상](#-소개-영상)  
5️⃣ [🚀 사용 환경 설정 및 시작하기](#-사용-환경-설정-및-시작하기)  
6️⃣ [🌐 시스템 아키텍처](#-시스템-아키텍처)  
7️⃣ [🛠 기술 스택](#-기술-스택)  
8️⃣ [👨‍👨‍👦‍👦 팀원 소개](#-팀원-소개)  

<br>
<br>

## ✨ 프로젝트 소개

'**MAPLE**'은 AI 기반 기타 연주 분석 및 피드백 시스템을 제공한다. 해당 시스템은 기타를 혼자 연습하고자 하는 사용자에게 개별 맞춤형 피드백을 제공한다. 이때, 피드백은 학습시킨 AI 모델을 활용해 연주의 뉘앙스(비브라토, 밴딩 등)까지 평가할 수 있다. 평가 후에는 문제점을 바로 시각화하여 사용자가 보완해야 할 부분, 완벽히 연주한 부분을 그래프 형태로 알려 준다. 원곡과 얼마나 비슷한지를 점수화하여 분석 결과를 제공해 주는 기능도 제공한다.

<br>
<br>

## 🎯 프로젝트 추진 배경 및 목적

최근 한국 대중음악 시장에서는 밴드 음악의 인기가 빠르게 증가하고 있다. 밴드 음악의 인기가 높아지면서 자연스레 악기 연습에 대한 관심이 증가했는데, 밴드에서 가장 핵심적인 역할을 하는 악기 중 하나인 기타의 관심도가 높았다. 많은 사람들이 배우고 싶어 하지만 실제 연습 과정에서 어려움을 겪는 경우가 많다. 기존의 방식들에는 맞춤형 피드백이 부족하며, 혼자 연습하는 경우에는 틀린 부분을 반복하며 잘못된 습관을 굳힐 수 있는 리스크가 크다. 또한, 코드 전환이나 세부적인 표현을 연습할 때에도 정확한 피드백을 받기 어렵기 때문에 이 서비스를 제안한다.

<br>
<br>

## 🦾 주요 기능

- 🎧 연주한 음원에 대한 AI 기반 피드백 제공  
- 📊 연주한 음원의 음정, 박자, 테크닉을 분석하여 시각적 그래프로 표현  
- 📈 연주한 음원 구간별 연주 정확도 분석 및 시각화  
- 🎼 레퍼런스 음원 재생 및 TAB 악보 동기화 제공  
- 🔐 이메일 및 구글 소셜 로그인 지원  
- 🔎 Spotify 기반 곡 추천 및 검색 기능  
- 💬 커뮤니티 기반 사용자 간 소통 기능  

<br>
<br>

## 🎬 소개 영상

[📺 MAPLE 소개 영상 보러가기](https://youtu.be/vBwf0QZoc_4)

<br>
<br>

## 🚀 사용 환경 설정 및 시작하기

### Docker 기반 실행

```bash
# 프로젝트 클론
git clone https://github.com/kookmin-sw/capstone-2025-05.git
cd capstone-2025-05

# 환경 변수 설정 (예: .env 파일)
cp .env.example .env

# Docker 이미지 빌드 및 실행
docker-compose up --build
```
<br>
<br>

## 🌐 시스템 아키텍처

<div align="center">
  <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/system architecture_01.png" width="100%" alt="System Architecture"/>
</div>

<br>
<br>

## 🛠 기술 스택

### 🖥 프론트엔드

| 역할 | 종류 |
|------|------|
| Programming Language | ![Javascript](https://img.shields.io/badge/Javascript-F7DF1E.svg?style=for-the-badge&logo=javascript&logoColor=white) |
| Library | ![React](https://img.shields.io/badge/React-61DAFB?style=for-the-badge&logo=react&logoColor=white) |
| Data Fetching | ![React Query](https://img.shields.io/badge/React_Query-FF4154?style=for-the-badge&logo=reactquery&logoColor=white) |
| Styling | ![TailwindCSS](https://img.shields.io/badge/tailwindcss-%2338B2AC.svg?style=for-the-badge&logo=tailwind-css&logoColor=white) |
| API | ![Spotify](https://img.shields.io/badge/Spotify-1ED760?style=for-the-badge&logo=spotify&logoColor=white) |
| Package Manager | ![Yarn](https://img.shields.io/badge/Yarn-2C8EBB.svg?style=for-the-badge&logo=Yarn&logoColor=white) |
| Web Server | ![Nginx](https://img.shields.io/badge/Nginx-009639.svg?style=for-the-badge&logo=Nginx&logoColor=white) |

### ⚙ 백엔드

| 역할 | 종류 |
|------|------|
| Programming Language | ![Python](https://img.shields.io/badge/Python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54) |
| Framework | ![FastAPI](https://img.shields.io/badge/FastAPI-005571?style=for-the-badge&logo=fastapi) |
| Auth / Storage | ![Firebase](https://img.shields.io/badge/Firebase-FFCA28?style=for-the-badge&logo=Firebase&logoColor=white) |

### 🖥 데스크탑 앱

| 구성 | 종류 |
|------|------|
| Framework | JUCE |
| Language | C++ |
| Build System | CMake |

### 🧠 AI / 분석

| 구성요소 | 종류 |
|----------|------|
| 음정 인식 | FFT 기반 + ML 보정 |
| 리듬 분석 | Onset Detection + Madmom |
| 결과 포맷 | JSON 기반 시각화 |
| Task Queue | ![Celery](https://img.shields.io/badge/Celery-37814A?style=for-the-badge&logo=Celery&logoColor=white) |
| Message Broker | ![Redis](https://img.shields.io/badge/Redis-DC382D?style=for-the-badge&logo=redis&logoColor=white) |
| DB | ![MongoDB](https://img.shields.io/badge/MongoDB-47A248?style=for-the-badge&logo=mongodb&logoColor=white) |
| Serving | ![BentoML](https://img.shields.io/badge/BentoML-FF5B00?style=for-the-badge&logo=bentoml&logoColor=white) |

<br>
<br>

## 👨‍👨‍👦‍👦 팀원 소개

| 사진 | 이름 | 역할 | GitHub | 이메일 |
|------|------|------|--------|--------|
| <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/송혜원.jpg" width="100px"/> | **송혜원 (팀장)** | Backend, DB 관리 | [@songhyeone](https://github.com/20223096) | won10201@kookmin.ac.kr |
| <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/신수민.jpg" width="100px"/> | **신수민** | Backend, DB 관리 | [@suminshin](https://github.com/syngrxm) | smcubez48@gmail.com |
| <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/박재훈.jpg" width="100px"/> | **박재훈 (PM)** | 데스크탑 클라이언트, AI 기반 오디오 분석 서버, CI/CD 파이프라인 구축 | [@jaehoonpark](https://github.com/lovelyoverflow) | hoon20@kookmin.ac.kr |
| <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/김도경.jpg" width="100px"/> | **김도경** | Frontend, UI/UX 디자인 | [@dokyoungkim](https://github.com/dgKim1) | okdk66833@kookmin.ac.kr |
| <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/박지민.jpg" width="100px"/> | **박지민** | Frontend, UI/UX 디자인 | [@jiminpark](https://github.com/qkrwlalss) | wimiji@kookmin.ac.kr |
| <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/이정윤.jpg" width="100px"/> | **이정윤** | Frontend, UI/UX 디자인 | [@jungyoonlee](https://github.com/lyun1015) | lyun1015@kookmin.ac.kr |
