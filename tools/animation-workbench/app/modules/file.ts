export const readFile = async (file: File) => {
  const fileReader = new FileReader()

  return new Promise<string>((resolve, reject) => {
    fileReader.addEventListener('load', (e) => {
      if (!e.target) {
        reject(new Error('Invalid target'))
        return;
      }

      resolve(e.target.result as string);
    })

    fileReader.addEventListener('error', () => {
      reject(new Error('Error reading file'))
    });

    fileReader.readAsText(file);
  });
}
