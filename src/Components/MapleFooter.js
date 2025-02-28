import React from 'react';
import { IoLogoGithub } from 'react-icons/io5';
import { MdEmail } from 'react-icons/md';
import { MdCopyright } from 'react-icons/md';

const MapleFooter = ({ github, email }) => {
  return (
    <div className="flex justify-center items-center py-[15px]">
      <p className="flex">
        <span className="flex">
          <div className="flex items-center font-bold">
            <MdCopyright /> 2024 MAPLE
          </div>
          . All Rights Reserved. |
        </span>
        &nbsp;
        <span className="flex items-center">
          <div className="flex font-bold items-center">
            <IoLogoGithub />
            GitHub
          </div>
          {`: ${github} |`}
        </span>
        &nbsp;
        <span className="flex items-center">
          <div className="flex font-bold items-center">
            <MdEmail />
            Email
          </div>
          {`: ${email}`}
        </span>
      </p>
    </div>
  );
};

export default MapleFooter;
