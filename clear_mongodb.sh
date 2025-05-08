#!/bin/bash

# MongoDB 컨테이너 이름 (필요 시 변경)
MONGO_CONTAINER="maple-audio-analyzer-mongo-1"

# 색상 정의
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}MongoDB 데이터베이스 정리 스크립트${NC}"
echo "-----------------------------"

# MongoDB 컨테이너가 실행 중인지 확인
echo -e "${YELLOW}MongoDB 컨테이너 확인 중...${NC}"
if ! docker ps | grep -q $MONGO_CONTAINER; then
  echo -e "${RED}오류: MongoDB 컨테이너($MONGO_CONTAINER)가 실행 중이 아닙니다.${NC}"
  exit 1
fi

# 현재 데이터베이스 목록 확인
echo -e "${YELLOW}현재 데이터베이스 목록:${NC}"
docker exec -it $MONGO_CONTAINER mongosh --quiet --eval "db.adminCommand('listDatabases').databases.forEach(function(d) { print(' - ' + d.name); })"

# 사용자에게 삭제 확인 요청
echo ""
echo -e "${RED}경고: 사용자 데이터베이스를 모두 삭제합니다.${NC}"
read -p "계속 진행하시겠습니까? (y/n): " choice

if [ "$choice" != "y" ] && [ "$choice" != "Y" ]; then
  echo -e "${YELLOW}작업이 취소되었습니다.${NC}"
  exit 0
fi

# 시스템 데이터베이스를 제외한 모든 데이터베이스 삭제
echo -e "${YELLOW}사용자 데이터베이스 삭제 중...${NC}"
docker exec -it $MONGO_CONTAINER mongosh --quiet --eval '
  db.adminCommand("listDatabases").databases.forEach(function(d) {
    const name = d.name;
    if(name !== "admin" && name !== "config" && name !== "local") {
      db = db.getSiblingDB(name);
      const result = db.dropDatabase();
      if(result.ok === 1) {
        print("데이터베이스 삭제 완료: " + name);
      } else {
        print("데이터베이스 삭제 실패: " + name);
      }
    }
  });
'

# 삭제 후 데이터베이스 목록 확인
echo ""
echo -e "${YELLOW}남아있는 데이터베이스 목록:${NC}"
docker exec -it $MONGO_CONTAINER mongosh --quiet --eval "db.adminCommand('listDatabases').databases.forEach(function(d) { print(' - ' + d.name); })"

echo ""
echo -e "${GREEN}작업이 완료되었습니다.${NC}" 