"""Write tests/fixtures/serial_cases.json: event logs from the REAL serial-control sketches (compiled on the desktop by
scripts/serial_harness.py) for a set of byte streams. web/static/serial_engine.test.mjs checks the JS port reproduces
every log exactly; tests/test_web.py checks this file is still what the sketches produce.

Run: python scripts/make_serial_fixture.py
"""
import json
from pathlib import Path

import serial_harness as sh

OUT = sh.ROOT / "tests" / "fixtures" / "serial_cases.json"
CASES = [  # (sketch, variant, stream, gap)
    ("makeblock", "fixed", "$L200Z$R-90Z", 0), ("makeblock", "fixed", "$L4Z$R900Z", 0),
    ("makeblock", "fixed", "$L-300Z", 0), ("makeblock", "fixed", "$X12Z$L12Z", 0),
    ("makeblock", "fixed", "junk$L50Z", 0), ("makeblock", "fixed", "$R+77Z$L 42Z", 0),
    ("makeblock", "fixed", "$L12abcZ", 0), ("makeblock", "fixed", "$L200Z$R-90Z", 2),
    ("makeblock", "fixed", "$R-255Z", 1), ("makeblock", "fixed", "$L20", 0),
    ("makeblock", "original", "$L200Z", 2), ("makeblock", "original", "$L200Z$R-90Z", 0),
    ("makeblock", "original", "$L12Z", 1), ("makeblock", "original", "$L20", 0),
    ("adafruit", "fixed", "$L300Z$T190Z", 0), ("adafruit", "fixed", "$L-120Z$R60Z$B3Z", 0),
    ("adafruit", "fixed", "$B90Z$T0Z", 0), ("adafruit", "fixed", "$L200Z$B120Z", 2),
    ("adafruit", "original", "$L200Z$B120Z", 2),
]


def make():
    run_case, cleanup = sh.runner()
    try:
        return [{"sketch": s, "variant": v, "stream": stream, "gap": gap, "events": run_case(s, v, stream, gap)}
                for s, v, stream, gap in CASES]
    finally:
        cleanup()


if __name__ == "__main__":
    data = make()
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps(data, indent=0) + "\n")
    print(f"{len(data)} cases, {sum(len(c['events']) for c in data)} events -> {OUT.relative_to(sh.ROOT)}")
