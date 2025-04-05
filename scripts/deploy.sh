#!/bin/bash

# 배포 스크립트
echo "===== MAPLE 애플리케이션 배포 시작 ====="

# 깃 저장소에서 최신 코드 가져오기
echo "Git 저장소에서 최신 코드 가져오는 중..."
git pull origin main || git pull origin master

# Docker 이미지 빌드 및 컨테이너 실행
echo "Docker 컨테이너 재시작 중..."
docker-compose down
docker-compose up -d --build

echo "===== 배포 완료 ====="