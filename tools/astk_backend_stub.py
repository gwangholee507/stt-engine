"""Stand-in for the web backend: records every audiostackd HTTP POST (session start, transcript, end) as one JSON line.
Answers 201/202/200 like docs/INTEGRATION-WEB.md so the daemon's queue never stalls. Used by integration_board.sh.
  python3 astk_backend_stub.py --port 3999 --out backend_posts.jsonl"""
import argparse, json, time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ap = argparse.ArgumentParser()
ap.add_argument("--port", type=int, default=3999)
ap.add_argument("--out", default="backend_posts.jsonl")
a = ap.parse_args()
out = open(a.out, "a", encoding="utf-8")


class H(BaseHTTPRequestHandler):
    def do_POST(self):
        n = int(self.headers.get("Content-Length") or 0)
        raw = self.rfile.read(n).decode("utf-8", "replace")
        try: body = json.loads(raw)
        except ValueError: body = {"_raw": raw}
        out.write(json.dumps({"t": time.monotonic(), "path": self.path, "body": body}, ensure_ascii=False) + "\n"); out.flush()
        code = 201 if self.path == "/api/sessions" else 202 if self.path.endswith("/transcripts") else 200
        self.send_response(code); self.send_header("Content-Type", "application/json"); self.end_headers()
        self.wfile.write(b'{"ok":true}')

    def log_message(self, *args): pass


print(f"backend stub on 127.0.0.1:{a.port} -> {a.out}", flush=True)
ThreadingHTTPServer(("127.0.0.1", a.port), H).serve_forever()
