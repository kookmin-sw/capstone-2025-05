import React, { useEffect, useState } from 'react';
import { useSearchParams } from 'react-router';
import { useSearchPlaylistsQuery } from '../../Hooks/get/useSearchPlaylistsQuery';
import Playbox from '../../Components/Playbox/Playbox';
import MapleHeader from '../../Components/MapleHeader';

export default function SearchPage() {
  const [searchParams] = useSearchParams();
  const searchKeyword = searchParams.get('keyword') || '검색어 없음';

  const { data: playlists } = useSearchPlaylistsQuery(searchKeyword);
  const [playerTarget, setPlayerTarget] = useState();
  return (
    <>
      <MapleHeader />
      <div className="flex ml-32 mb-8 mt-12">
        <h1 className="font-bold text-xl">
          "{searchKeyword}"에 대한 검색결과 입니다.
        </h1>
      </div>
      <div className="grid gap-12 lg:grid-cols-4 md:grid-cols-3 sm: grid-cols-2 ml-32 pb-16">
        {playlists &&
          playlists.map((track) => (
            <Playbox
              img={track.album.images[0].url}
              title={track.name}
              artist={track.artists[0].name}
              playurl={track.uri}
              playerTarget={playerTarget}
              setPlayerTarget={setPlayerTarget}
              isSelected={
                playerTarget && track.name == playerTarget.title ? true : false
              }
            />
          ))}
      </div>
    </>
  );
}
