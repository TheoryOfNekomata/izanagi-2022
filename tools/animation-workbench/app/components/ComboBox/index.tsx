import {FormEventHandler, forwardRef, HTMLProps, ReactNode, useId} from 'react';

export interface ComboBoxProps extends Omit<HTMLProps<HTMLInputElement>, 'list'> {
  onOptionSelect?: FormEventHandler<HTMLInputElement>;
  onCustomValueInput?: FormEventHandler<HTMLInputElement>;
  children?: ReactNode;
}

export const ComboBox = forwardRef<HTMLInputElement, ComboBoxProps>(({
  onOptionSelect,
  children,
  className = '',
  onInput,
  onCustomValueInput,
  ...etcProps
}, ref) => {
  const id = useId();
  const handleInput: FormEventHandler<HTMLInputElement> = (e) => {
    onInput?.(e);
    const nativeEvent = e.nativeEvent as unknown as { inputType?: string };
    switch (nativeEvent.inputType) {
    case 'deleteContentBackward':
    case 'deleteContentForward':
    case 'insertText':
      // we type into the input
      onCustomValueInput?.(e);
      return;
    default:
      break;
      // we select from the datalist

    }
    onOptionSelect?.(e);
  }

  return (
    <>
      <datalist
        id={id}
      >
        {children}
      </datalist>
      <input
        {...etcProps}
        ref={ref}
        list={id}
        className={`w-full block h-12 px-4 ${className}`}
        onInput={handleInput}
      />
    </>
  )
})
