import React from 'react';
import musicIcon from '../../Assets/music-note.svg';
import Playbox from '../../Components/Playbox/Playbox';
import { useState } from 'react';
import { useLocation } from 'react-router-dom';

export default function MusicPage() {
  const location = useLocation();
  const { musics, musicType } = location.state;
  const [playerTarget, setPlayerTarget] = useState();
  return (
    <>
      <div className="flex ml-32 mb-8 mt-12">
        <h1 className="font-bold text-2xl">
          <div className="flex items-center">
            <img src={musicIcon} alt="음악" className="w-12 mr-4" />
            {musicType === 'top'
              ? 'Top 20 Popular Songs'
              : 'Top 10 New Releases'}
          </div>
        </h1>
      </div>
      <div className="grid gap-12 lg:grid-cols-4 md:grid-cols-3 sm: grid-cols-2 ml-32 pb-16">
        {musics.map((music) => (
          <Playbox
            img={music.cover}
            title={music.title}
            artist={music.artist}
            playurl={music.uri}
            playerTarget={playerTarget}
            setPlayerTarget={setPlayerTarget}
            isSelected={
              playerTarget && music.title == playerTarget.title ? true : false
            }
          />
        ))}
      </div>
    </>
  );
}
