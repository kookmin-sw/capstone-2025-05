import React from 'react';

export default function Input(props) {
  const commonProps = {
    className:
      'border border-[#A57865] cursor-pointer rounded-lg p-2 outline-none',
    style: {
      width: props.width,
      height: props.height,
    },
    placeholder: props.placeholder,
    onClick: props.onClick,
    onChange: props.onChange,
    value: props.value,
    onKeyDown: props.onKeyDown,
  };

  return props.type === 'textarea' ? (
    <textarea
      {...commonProps}
      className={`${commonProps.className} resize-none`}
    />
  ) : (
    <input {...commonProps} type={props.type} />
  );
}
