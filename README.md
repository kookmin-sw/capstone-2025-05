# 🎸 <span style="color:#A57865"><strong>AI 기반 기타 연주 분석 플랫폼</strong></span> - [MAPLE](https://maple.ne.kr/)

<div align="center"> 
  <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/logo.svg" width="70%" alt="MAPLE Logo"/>
</div>



## 📌 목차

1️⃣ [💡 프로젝트 소개](#-프로젝트-소개)  

2️⃣ [🦾 주요 기능](#-주요-기능)  

3️⃣ [🎬 시연 영상](#-시연-영상)  

4️⃣ [👨‍👨‍👦‍👦 팀원 소개](#-팀원-소개)  

5️⃣ [🌐 시스템 구조](#-시스템-구조)  

6️⃣ [🛠 기술 스택](#-기술-스택)  

7️⃣ [🚀 실행 방법](#-실행-방법)  

8️⃣ [📂 폴더 구조](#-폴더-구조)  

9️⃣ [📎 참고 자료](#-참고-자료)

---

## ✨ 프로젝트 소개

<div align="center"> 
  <img src="https://raw.githubusercontent.com/kookmin-sw/capstone-2025-05/master/assets/Maple.png" width="100%" alt="MAPLE App"/>
</div>

> **MAPLE** (Music Analysis & Practice Learning Environment)은 기타 연주 학습자와 밴드 연습자들을 위한 AI 기반 실시간 연주 분석 및 피드백 시스템입니다.
> 
-✏️사용자의 기타 연주를 실시간으로 분석하여 **피치 정확도**, **리듬 정확도**, **연주 안정성** 등 다양한 측면의 피드백을 시각적으로 제공하는 AI 기반 웹 플랫폼입니다.
-🎵 **2024년, 록·밴드 음악 인기 급상승**: DAY6·실리카겔·Wave to Earth 등 인디 밴드와 대중 밴드 모두 빠르게 성장 중
-📉 기존 학습 도구의 한계: 유튜브·악보·튜너 등은 실시간 피드백 부족
-🧠 MAPLE은 실시간 음정·박자·테크닉 분석과 함께 개별 피드백을 제공
-🎯 초보자부터 전문가까지, 자기 연주를 **정량적으로 분석하고 성장**할 수 있도록 돕습니다.  
## 👨‍👨‍👦‍👦 팀원 소개
| 사진 | 이름 | 역할 | GitHub | 이메일 |
|------|------|------|--------|--------|
| ![]() | **송혜원** | Backend(팀장) | [@songhyeone](https://github.com/20223096) | hyeone@song.com |
| ![]() | **박재훈** | AI | [@jaehoonpark](https://github.com/lovelyoverflow) | jaehoon@maple.com |
| ![]() | **신수민** | Backend | [@soominshin](https://github.com/syngrxm) | soomin@maple.com |
| ![]() | **이정윤** | Frontend | [@jungyoonlee](https://github.com/lyun1015 ) | jungyoon@maple.com |
| ![]() | **김도경** | Frontend | [@dokyoungkim](https://github.com/dgKim1) | dokyoung@maple.com |
| ![]() | **박지민** | Frontend | [@jiminpark](https://github.com/qkrwlalss) | jimin@maple.com |

## 🛠 기술 스택

### 🖥 프론트엔드

|역할|종류|
|-|-|
|**Programming Language**| ![Javascript](https://img.shields.io/badge/Javascript-F7DF1E.svg?style=for-the-badge&logo=javascript&logoColor=white)
|**Library**| ![React](https://img.shields.io/badge/React-61DAFB?style=for-the-badge&logo=react&logoColor=white)
|**Data Fetching Library**|![React Query](https://img.shields.io/badge/React_Query-FF4154?style=for-the-badge&logo=reactquery&logoColor=white)
|**Styling**| ![tailwindcss](https://img.shields.io/badge/tailwindcss-%2338B2AC.svg?style=for-the-badge&logo=tailwind-css&logoColor=white)
|**API**| ![tailwindcss](https://img.shields.io/badge/Spotify-1ED760?style=for-the-badge&logo=spotify&logoColor=white)
|**Package Manager**|![Yarn](https://img.shields.io/badge/Yarn-2C8EBB.svg?style=for-the-badge&logo=Yarn&logoColor=white)                             
|**Web Server**|![Nginx](https://img.shields.io/badge/Nginx-009639.svg?style=for-the-badge&logo=Nginx&logoColor=white)

### ⚙ 백엔드

<img src="https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54"/>
<img src="https://img.shields.io/badge/FastAPI-005571?style=for-the-badge&logo=fastapi"/>
<img src="https://img.shields.io/badge/firebase-%23039BE5.svg?style=for-the-badge&logo=firebase"/>

### 🧠 AI / 분석

- 음정 인식 모델 (FFT 기반 + ML 보정)
- 리듬 분석 알고리즘 (Onset detection + Madmom)
- JSON 기반 시각화 데이터 반환

---

## 🚀 실행 방법

