FROM node:20 AS build

# build-args 전달받기
ARG REACT_APP_SPOTIFY_CLIENT_ID
ARG REACT_APP_SPOTIFY_CLIENT_SECRET
ARG REACT_APP_SPOTIFY_REDIRECT_URI
ARG REACT_APP_API_DATABASE_URL
ARG REACT_APP_CLIENT_ID
ARG REACT_APP_REDIRECT_URI
ARG REACT_APP_SCOPE
ARG PUBLIC_URL
ARG REACT_APP_ANALYSIS_URL
ARG REACT_APP_MEDIA_URL
ARG REACT_APP_COVER_URL

WORKDIR /app

# 패키지 설치
COPY package.json yarn.lock ./
RUN yarn install

# 소스 복사
COPY . .

#.env.production 파일 생성 (CRA가 이걸 읽음)
RUN echo "REACT_APP_SPOTIFY_CLIENT_ID=$REACT_APP_SPOTIFY_CLIENT_ID" >> .env.production && \
    echo "REACT_APP_SPOTIFY_CLIENT_SECRET=$REACT_APP_SPOTIFY_CLIENT_SECRET" >> .env.production && \
    echo "REACT_APP_SPOTIFY_REDIRECT_URI=$REACT_APP_SPOTIFY_REDIRECT_URI" >> .env.production && \
    echo "REACT_APP_API_DATABASE_URL=$REACT_APP_API_DATABASE_URL" >> .env.production && \
    echo "REACT_APP_CLIENT_ID=$REACT_APP_CLIENT_ID" >> .env.production && \
    echo "REACT_APP_REDIRECT_URI=$REACT_APP_REDIRECT_URI" >> .env.production && \
    echo "REACT_APP_SCOPE=$REACT_APP_SCOPE" >> .env.production && \
    echo "REACT_APP_ANALYSIS_URL=$REACT_APP_ANALYSIS_URL" >> .env.production && \
    echo "REACT_APP_MEDIA_URL=$REACT_APP_MEDIA_URL" >> .env.production && \
    echo "REACT_APP_COVER_URL=$REACT_APP_COVER_URL" >> .env.production && \
    echo "PUBLIC_URL=$PUBLIC_URL" >> .env.production

# CRA 빌드
RUN yarn build

# Nginx 이미지로 전환
FROM nginx:alpine

# 빌드된 정적 파일 복사
COPY --from=build /app/build /usr/share/nginx/html

# Nginx 설정 복사
COPY nginx.conf /etc/nginx/conf.d/default.conf
