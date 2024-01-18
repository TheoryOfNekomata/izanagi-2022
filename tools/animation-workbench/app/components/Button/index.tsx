import {forwardRef, HTMLProps, ReactNode} from 'react';

export interface ButtonProps extends Omit<HTMLProps<HTMLButtonElement>, 'type'> {
  type?: 'button' | 'reset' | 'submit';
}

export const Button = forwardRef<HTMLButtonElement, ButtonProps>(({
  className = '',
  ...etcProps
}, ref) => (
  <button
    {...etcProps}
    ref={ref}
    className={`h-12 px-4 border-2 rounded ${className}`}
  />
))
