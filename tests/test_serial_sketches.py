"""Run the real serial-control sketches (compiled on the desktop with tests/arduino_harness) and check how they parse
commands. Regression test for the command reader reading -1 while a command's bytes are still arriving.
"""
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import serial_harness as sh  # noqa: E402

pytestmark = pytest.mark.skipif(not sh.compiler(), reason="no C++ compiler")


@pytest.fixture(scope="module")
def run_case():
    fn, cleanup = sh.runner()
    yield fn
    cleanup()


def last(events, prefix):
    return [e for e in events if e.startswith(prefix)][-1]


def test_buffered_command_sets_motors(run_case):
    ev = run_case("makeblock", "fixed", "$L200Z$R-90Z")
    assert last(ev, "M1.") == "M1.run(200)" and last(ev, "M2.") == "M2.run(-90)"


def test_deadband_and_clamp(run_case):
    ev = run_case("makeblock", "fixed", "$L4Z$R900Z")
    assert last(ev, "M1.") == "M1.stop()" and last(ev, "M2.") == "M2.run(255)"


@pytest.mark.parametrize("sketch,expect", [("makeblock", "M1.run(200)"), ("adafruit", "AF3.setSpeed(200)")])
def test_command_arriving_byte_by_byte(run_case, sketch, expect):
    # bytes 2 polls apart: before the fix the reader stored -1 (0xFF) for the missing bytes and dropped the command
    ev = run_case(sketch, "fixed", "$L200Z", gap=2)
    assert expect in ev


def test_original_reader_dropped_slow_commands(run_case):
    ev = run_case("makeblock", "original", "$L200Z", gap=2)
    assert "M1.run(200)" not in ev and "TX:0" in ev


def test_every_copy_of_the_reader_waits_for_bytes():
    for p in ROOT.rglob("*.ino"):
        if "libraries" in p.parts:
            continue
        text = p.read_text(errors="replace")
        n_reads = text.count("while(current_char != command_end)")
        assert text.count(sh.WAIT_MARK) >= n_reads, p
