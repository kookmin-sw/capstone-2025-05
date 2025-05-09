import requests
import base64
import sys
import os

def get_access_token(client_id, client_secret):
    url = 'https://accounts.spotify.com/api/token'
    auth_header = base64.b64encode(f'{client_id}:{client_secret}'.encode()).decode()
    headers = {
        'Authorization': f'Basic {auth_header}',
    }
    data = {
        'grant_type': 'client_credentials',
    }
    response = requests.post(url, headers=headers, data=data)
    response.raise_for_status()
    return response.json()['access_token']

def search_tracks(query, token):
    url = 'https://api.spotify.com/v1/search'
    headers = {
        'Authorization': f'Bearer {token}',
    }
    params = {
        'q': query,
        'type': 'track',
        'limit': 10,  # 여러 결과 보기
    }
    response = requests.get(url, headers=headers, params=params)
    response.raise_for_status()
    return response.json()['tracks']['items']

def download_image(url, filename):
    response = requests.get(url)
    response.raise_for_status()
    with open(filename, 'wb') as f:
        f.write(response.content)
    print(f'다운로드 완료: {filename}')

def main():
    client_id = '8ac9afb74e724f86907d9cfe8ba9a282'
    client_secret = 'ad069435f5d841998715dc4a607e55b3'

    if len(sys.argv) < 2:
        print("사용법: python script.py '곡 제목 [아티스트명]'")
        sys.exit(1)

    query = sys.argv[1]
    token = get_access_token(client_id, client_secret)
    tracks = search_tracks(query, token)

    if not tracks:
        print("검색 결과가 없습니다.")
        sys.exit(1)

    print("검색 결과:")
    for idx, track in enumerate(tracks):
        title = track['name']
        artists = ", ".join([artist['name'] for artist in track['artists']])
        album_name = track['album']['name']
        album_type = track['album']['album_type']
        release_date = track['album']['release_date']
        print(f"[{idx}] {title} - {artists} | 앨범: {album_name} | 타입: {album_type} | 출시일: {release_date}")

    choice = int(input("다운로드할 번호를 입력하세요: "))
    selected_track = tracks[choice]
    image_url = selected_track['album']['images'][0]['url']
    
    title = selected_track['name'].replace('/', '_').replace('\\', '_')
    artist = selected_track['artists'][0]['name'].replace('/', '_').replace('\\', '_')
    album = selected_track['album']['name'].replace('/', '_').replace('\\', '_')
    
    filename = f"{title}-{artist}.jpg"
    download_image(image_url, filename)

if __name__ == '__main__':
    main()

