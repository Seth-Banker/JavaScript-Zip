# JavaScript-Zip

**Windows only** for now.

Node.js tool to create ZIP files in JavaScript with native C++ speed using Miniz. License is MIT.

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

// Create a ZIP from FILES note.txt and image.png then stores the new ZIP as zipFile in memory to be used later.
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

### `unzip(zipBytes: Buffer): Record<string, Buffer>` 
reads a ZIP already in memory. Returns an object where each key is the path inside the ZIP and each value is a Node Buffer.

```js
const fs = require('fs');
const { unzip } = require('javascript-zip');

// find our target zip file named 'bundle.zip'
const zipBytes = fs.readFileSync('./bundle.zip');

// zip is now in memory as 'files'
const files = unzip(zipBytes);

// 'files' is a list of key/value pairs of files paths/byte data.

// pull a specific file named 'note.txt'
if (files['note.txt']) {
  fs.writeFileSync('./unpackedNote.txt', files['note.txt']);
}
```

### `unzipDisk(zipBytes: Buffer, outDir: string): void`
extracts all regular files from a ZIP in memory to outDir. Creates folders as needed.

***This will pull the ZIP into memory THEN write it out to disk.***

```js
const fs = require('fs');
const path = require('path');
const { unzipDisk } = require('javascript-zip');

// THIS PULLS THE ZIP INTO MEMORY
const zipBytes = fs.readFileSync('./bundle.zip');

// creates our directory named 'out' where we will write out the ZIPs files to.
const outDir = path.resolve(__dirname, 'out');

unzipDisk(zipBytes, outDir);  // writes out/note.txt and out/image.png
```

# Roadmap
- Add Web assembly support.
- Add ability to compress zips for server-client transfer and vice versa.
