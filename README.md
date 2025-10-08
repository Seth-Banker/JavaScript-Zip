# JavaScript-Zip

**Windows only** for now.

Create ZIP files in JavaScript with native C++ speed using Miniz. License is MIT.

---

## Install

```bash
npm install javascript-zip
```

## Functions

### `zip(files: string[]): Buffer`

builds a valid ZIP archive in memory from a list of file paths. It returns a Node Buffer with the ZIP bytes. It throws on missing files or I/O errors.

```js
const fs = require('fs');
const { zip } = require('javascript-zip');

// Create a ZIP from FILES note.txt and image.png then stores the new ZIP as zipFile.
const zipFile = zip([
  './note.txt',
  './image.png'
]);

// now zipped file is sitting in memory as 'zipFile'

// Writes the in-memory ZIP (zipFile) to disk as "bundle.zip"
fs.writeFileSync('bundle.zip', zipFile);
```

### `zipDisk(files: string[], outPath: string): void`
creates a ZIP archive on disk from a list of file paths. It writes the archive to outPath and returns nothing. It throws on missing files or write errors. It is the disk variant of zip() and avoids holding the whole ZIP in memory.

```js
const { zipDisk } = require('javascript-zip');

// 1st argument is an array of files. 2nd argument is the outpath. 
// This takes note.txt and image.png and writes them to a zip at "bundle.zip" in the same folder.
zipDisk(
  [
    './note.txt',
    './image.png'
  ],
  './bundle.zip'
);
```

`zipDisk()` is a faster way of writing the `zip()` example and avoid larger RAM usage. You would usually use `zip()` in cases when serving the ZIP or storing in a SQL or Redis.
