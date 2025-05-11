import React from 'react';
import musicIcon from '../../Assets/music-note.svg';
import { useLocation } from 'react-router-dom';
import Playbox from '../../Components/Playbox/Playbox';
import AudioPlaybox from '../../Components/Playbox/AudioPlaybox';
import NoDataPage from '../NoDataPage.js/NoDataPage';

export default function MusicPage() {
  const location = useLocation();
  const { musics, musicType, isLoading } = location.state;
  const COVER_URL = process.env.REACT_APP_COVER_URL;

  return (
    <>
      <div className="flex ml-32 mb-8 mt-12">
        <h1 className="font-bold text-2xl">
          <div className="flex items-center">
            <img src={musicIcon} alt="음악" className="w-12 mr-4" />
            {musicType === 'top'
              ? 'Top 20 Popular Songs'
              : musicType === 'analysis'
                ? 'Analysis Songs'
                : 'Top 10 New Releases'}
          </div>
        </h1>
      </div>
      <div className="grid gap-12 lg:grid-cols-4 md:grid-cols-3 sm: grid-cols-2 ml-32 pb-16">
        {isLoading && <h1>로딩중...</h1>}
        {musicType !== 'analysis' &&
          musics &&
          musics.map((music) => (
            <Playbox
              img={music.cover}
              title={music.title}
              artist={music.artist}
              playurl={music.uri}
            />
          ))}
        {musicType === 'analysis' &&
          musics &&
          musics.map((album) => (
            <AudioPlaybox
              img={COVER_URL + '/' + album.thumbnail}
              title={album.title}
              artist={album.artist}
              song_id={album.song_id}
              playurl={album.audio}
            />
          ))}
        {!musics && <NoDataPage />}
      </div>
    </>
  );
}
