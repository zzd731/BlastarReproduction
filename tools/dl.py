import sys, urllib.request

url, out = sys.argv[1], sys.argv[2]
req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"})
with urllib.request.urlopen(req, timeout=60) as r:
    data = r.read()
    print("http", r.status, "bytes", len(data))
    with open(out, "wb") as f:
        f.write(data)
print("saved", out)
