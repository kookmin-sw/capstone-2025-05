import React from 'react';

export default function Profile({ profile_url }) {
  return (
    <div className="h-[50px] w-[50px] rounded-full overflow-hidden shadow-md mr-2">
      <img src={profile_url} className="object-cover" />
    </div>
  );
}
