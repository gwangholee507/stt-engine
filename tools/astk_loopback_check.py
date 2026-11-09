"""Reads what the backend stub recorded (and the engine log) after a run through the real audiostackd, and reports
where results ended up. Used by integration_board.sh.
  python3 astk_loopback_check.py backend_posts.jsonl --engine-log engine.out [--ref tedx/talk04.txt]
Reports:
  - sessions the daemon announced and ended (POST /api/sessions, /end)
  - transcripts whose URL/envelope session differs from the session the engine put in the transcript
    (audiostackd attaches the session open when the RESULT arrives, src/daemon.cpp:483)
  - finals that reached the backend after that session's /end
  - seg_id 0..n per engine session, CER of all finals joined (if --ref), final latency from the engine log"""
import argparse, json, re, sys, pathlib
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))
from board_asr import edit_distance, norm

ap = argparse.ArgumentParser()
ap.add_argument("posts"); ap.add_argument("--engine-log"); ap.add_argument("--ref")
a = ap.parse_args()
P = [json.loads(l) for l in open(a.posts, encoding="utf-8") if l.strip()]
starts = [p for p in P if p["path"] == "/api/sessions"]
ends = {p["path"].split("/")[3]: p["t"] for p in P if p["path"].endswith("/end")}
tr = [p for p in P if p["path"].endswith("/transcripts")]
print(f"sessions started {len(starts)}: {[p['body'].get('session_id') for p in starts]}")
print(f"sessions ended   {len(ends)}: {list(ends)}")
print(f"transcript POSTs {len(tr)} (final {sum(p['body']['transcript'].get('type') == 'final' for p in tr)})")

mis = []
for p in tr:
    url_sid = p["path"].split("/")[3]; env_sid = p["body"].get("session_id"); eng_sid = p["body"]["transcript"].get("session_id")
    if not (url_sid == env_sid == eng_sid): mis.append((p["body"]["transcript"].get("type"), eng_sid, url_sid, p["body"]["transcript"].get("text", "")[:40]))
print(f"\nWRONG SESSION (daemon envelope != engine session_id): {len(mis)}")
for t, e, u, x in mis[:10]: print(f"  {t:7s} engine {e} -> posted under '{u}' | {x}")

late = [(p["body"]["transcript"].get("session_id"), p["t"] - ends[p["body"]["transcript"].get("session_id")], p["body"]["transcript"].get("text", "")[:40])
        for p in tr if p["body"]["transcript"].get("type") == "final" and p["body"]["transcript"].get("session_id") in ends
        and p["t"] > ends[p["body"]["transcript"].get("session_id")]]
print(f"FINALS AFTER THEIR SESSION'S /end: {len(late)}")
for s, d, x in late[:10]: print(f"  {s} +{d:.2f}s after /end | {x}")

by = {}
for p in tr:
    t = p["body"]["transcript"]
    if t.get("type") == "final": by.setdefault(t.get("session_id"), []).append(t.get("seg_id"))
for s, ids in by.items():
    print(f"seg_id {s}: {'OK 0..%d' % (len(ids) - 1) if ids == list(range(len(ids))) else 'NOT 0..n ' + str(ids[:12])}")
fins = [p["body"]["transcript"] for p in tr if p["body"]["transcript"].get("type") == "final"]
print(f"empty finals {sum(bool(t.get('empty')) for t in fins)}")
hyp = " ".join(t["text"] for t in fins if t.get("text"))
if a.ref:
    ref = open(a.ref, encoding="utf-8").read().strip(); e = edit_distance(norm(ref), norm(hyp))
    print(f"CER {e / len(norm(ref)) * 100:.1f}% ({e}/{len(norm(ref))} chars), all finals that reached the backend")
if a.engine_log:
    L = open(a.engine_log, encoding="utf-8", errors="replace").read()
    lat = [float(x) for x in re.findall(r"^\[\s*[\d.]+~\s*[\d.]+s.*?latency ([\d.]+)s queued", L, re.M)]
    if lat: print(f"final latency (end of segment audio at capture -> result sent) avg {sum(lat)/len(lat):.2f}s max {max(lat):.2f}s, n {len(lat)}")
    print(f"engine: reconnects {L.count('[astk] reconnected')}, session_done {L.count('[astk] session_done')}, errors {L.count('Traceback')}")
print(f"\n[hypothesis]\n{hyp}")
