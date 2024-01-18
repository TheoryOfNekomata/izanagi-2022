import {forwardRef, HTMLProps, ReactNode} from 'react';

export interface FileButtonProps extends Omit<HTMLProps<HTMLInputElement>, 'type'> {
  children?: ReactNode;
}

export const FileButton = forwardRef<HTMLInputElement, FileButtonProps>(({
  className = '',
  children,
  ...etcProps
}, ref) => (
  <label className="contents">
    <input
      {...etcProps}
      ref={ref}
      type="file"
      className="sr-only"
    />
    <span className={`h-12 px-4 border-2 rounded inline-flex justify-center items-center cursor-pointer ${className}`}>
      {children}
    </span>
  </label>
))
