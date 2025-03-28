import React from 'react';

export default function Input(props) {
  const commonProps = {
    className: `border border-[#A57865] cursor-pointer rounded-lg p-2 outline-none appearance-none ${props.className}`,
    style: {
      width: props.width,
      height: props.height,
      arrow: 'none',
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
  ) : props.type === 'select' ? (
    <select {...commonProps}>
      <option value="" disabled>
        {props.placeholder || '선택해 주세요'}
      </option>
      {props.options?.map((option) => (
        <option key={option.value} value={option.value}>
          {option.label}
        </option>
      ))}
    </select>
  ) : (
    <input {...commonProps} type={props.type} />
  );
}
