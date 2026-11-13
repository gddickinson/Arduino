// Node test: the JS port reproduces, event for event, what the real sketches do when compiled and run on the desktop
// (tests/fixtures/serial_cases.json from scripts/make_serial_fixture.py), for the fixed and the original reader.
// Run: node web/static/serial_engine.test.mjs
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { run, atoi, finalOutputs, makeblockPins } from './serial_engine.js';

const cases = JSON.parse(readFileSync(new URL('../../tests/fixtures/serial_cases.json', import.meta.url)));
let n = 0;
for (const c of cases) {
  const { events } = run(c.sketch, c.stream, { gap: c.gap, fixed: c.variant === 'fixed' });
  assert.deepEqual(events, c.events, `${c.sketch}/${c.variant} ${c.stream} gap=${c.gap}`);
  n += events.length;
}
// hand checks
const enc = s => [...s].map(ch => ch.charCodeAt(0));
assert.equal(atoi(enc(' -42xyz')), -42); assert.equal(atoi(enc('+7')), 7); assert.equal(atoi(enc('abc')), 0);
assert.equal(atoi(enc('0000000000000000012')), 1);  // toCharArray(temp, 19) keeps 18 chars
assert.deepEqual(finalOutputs(run('makeblock', '$L-80Z').events), { M1: -80, M2: 0 });
assert.deepEqual(makeblockPins(-80), { dir: 'LOW', pwm: 80 }); assert.deepEqual(makeblockPins(300), { dir: 'HIGH', pwm: 255 });
console.log(`serial engine ok (${cases.length} cases, ${n} events identical to the compiled sketches)`);
