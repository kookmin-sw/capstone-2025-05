import React from 'react';
import { FaAngleDown, FaAngleUp } from 'react-icons/fa6';
import { ClipLoader } from 'react-spinners';

export default function CommentAngleButton({
  isLoading,
  isShow,
  filterComments,
  comments,
}) {
  return (
    <>
      {comments?.length > 3 && (
        <div>
          {
            <ClipLoader
              color="#0a0a0a"
              loading={isLoading}
              size={30}
              aria-label="Loading Spinner"
              data-testid="loader"
            />
          }
          {!isShow ? (
            <FaAngleDown size={'30px'} onClick={() => filterComments(true)} />
          ) : (
            <FaAngleUp size={'30px'} onClick={() => filterComments(false)} />
          )}
        </div>
      )}
    </>
  );
}
