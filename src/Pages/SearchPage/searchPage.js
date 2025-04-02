import React, { useEffect } from 'react';
import { useSearchParams } from 'react-router';
import { useSearchPlaylistsQuery } from '../../Hooks/get/useSearchPlaylistsQuery';
import Playbox from '../../Components/Playbox/Playbox';
import MapleHeader from '../../Components/MapleHeader';

export default function SearchPage() {
  const [searchParams] = useSearchParams();
  const searchKeyword = searchParams.get('keyword') || '검색어 없음';

  const { data: playlists } = useSearchPlaylistsQuery(searchKeyword);
  console.log(playlists);
  return (
    <>
      <MapleHeader />
      <div className="flex ml-32 mb-8 mt-12">
        <h1 className="font-bold text-xl">
          "{searchKeyword}"에 대한 검색결과 입니다.
        </h1>
      </div>
      <div className="flex flex-wrap justify-center gap-12 ml-4 pb-16">
        {playlists &&
          playlists.map((track) => (
            <Playbox
              index={track.id}
              img={track.album.images[2].url}
              title={track.name}
              artist={track.artists[0].name}
            />
          ))}
      </div>
    </>
  );
}
