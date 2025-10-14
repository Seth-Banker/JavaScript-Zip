// Prefer a prebuilt binary if present, else fall back to node-gyp build output
const path = require('path');
function tryRequire(p) { try { return require(p); } catch { return null; } }

module.exports =
  tryRequire(path.join(__dirname, 'prebuilds', `${process.platform}-${process.arch}`, 'javascript-zip.node')) ||
  tryRequire(path.join(__dirname, 'build', 'Release', 'javascript-zip.node')) ||
  // Helpful error if neither exists
  (() => { throw new Error('Native binary not found. Run `npm rebuild` or install a prebuilt package.'); })();
