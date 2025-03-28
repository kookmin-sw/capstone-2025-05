import React from 'react';
import Box from '../Box/Box';

export default function Alert(props) {
  return (
    <div className={`w-[${props.width}] h-[${props.height}]`}>
      <Box overwrite={props.overwrite} backgroundColor={props.backgroundColor}>
        <strong className="ml-[10px]">{props.content}</strong>
      </Box>
    </div>
  );
}
