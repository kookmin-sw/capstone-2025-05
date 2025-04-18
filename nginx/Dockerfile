FROM nginx:stable-alpine

# 기본 Nginx 설정 삭제
RUN rm /etc/nginx/conf.d/default.conf

# 커스텀 설정 파일 복사
COPY nginx.conf /etc/nginx/conf.d/default.conf

# 포트 노출
EXPOSE 80 443