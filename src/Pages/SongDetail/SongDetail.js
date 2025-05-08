import React from 'react';
import { useParams } from 'react-router-dom';

export default function SongDetail() {
  const { songId } = useParams();
  return <div>{'곡아이디:' + songId}</div>;
}
