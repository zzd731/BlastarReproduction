// Minimal downloader: run with ELECTRON_RUN_AS_NODE=1 "D:\dsh\DeepSeek Harness.exe" dl.js <url> <out>
const fs = require('fs');
const url = process.argv[2];
const out = process.argv[3];
(async () => {
  const r = await fetch(url);
  if (!r.ok) { console.error('HTTP ' + r.status); process.exit(1); }
  const b = Buffer.from(await r.arrayBuffer());
  fs.writeFileSync(out, b);
  console.log('saved ' + out + ' ' + b.length + ' bytes');
})().catch(e => { console.error('ERR ' + e.message); process.exit(1); });
