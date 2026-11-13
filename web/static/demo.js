// Serial command demo: replays a byte stream through the JS port of the serial-control sketches (serial_engine.js,
// event-for-event identical to the compiled C++ in the tests) and shows what each motor and servo is told to do.
import { route, app, esc, list } from './core.js';
import { SKETCHES, run, finalOutputs, makeblockPins } from './serial_engine.js';

const PRESETS = {
  makeblock: [['$L200Z$R200Z', 'forward'], ['$L-150Z$R150Z', 'spin'], ['$L3Z$R-4Z', 'inside deadband'], ['$L900Z$R-900Z', 'clamped'], ['$X50Z$L12abcZ', 'unknown letter, junk digits']],
  adafruit: [['$L200Z$R200Z', 'forward'], ['$L-120Z$R60Z', 'curve'], ['$B30Z$T150Z', 'servos'], ['$B0Z$T999Z', 'servo limits'], ['$L300Z', 'clamped']],
};

const show = cmd => cmd.map(b => b === 255 ? '·' : String.fromCharCode(b)).join('');

function wheels(sketch, out) {
  const bar = (x, y, v, label) => {
    const h = Math.round(Math.abs(v) / 255 * 60), up = v >= 0;
    return `<rect x="${x - 14}" y="${y - 32}" width="28" height="64" rx="5" class="whl"/>
      ${v ? `<rect x="${x - 6}" y="${up ? y - h : y}" width="12" height="${h}" class="${up ? 'fw' : 'bw'}"/>` : ''}
      <text class="tm" x="${x}" y="${y + 50}" text-anchor="middle">${esc(label)}</text>`;
  };
  if (sketch === 'makeblock') {
    return `<svg viewBox="0 0 260 170" role="img" aria-label="Robot from above: left motor ${out.M1 ?? 0}, right motor ${out.M2 ?? 0}">
      <rect x="70" y="30" width="120" height="110" rx="10" class="body"/><text class="tm" x="130" y="22" text-anchor="middle">bars: run(+) up, run(−) down</text>
      ${bar(48, 85, out.M1 ?? 0, `M1 ${out.M1 ?? 0}`)}${bar(212, 85, out.M2 ?? 0, `M2 ${out.M2 ?? 0}`)}</svg>`;
  }
  const v = p => { const m = out['AF' + p] || {}; return (m.dir === 'FORWARD' ? 1 : m.dir === 'BACKWARD' ? -1 : 0) * (m.speed || 0); };
  return `<svg viewBox="0 0 260 230" role="img" aria-label="Adafruit shield ports 1 to 4: run direction and speed">
    <rect x="70" y="30" width="120" height="170" rx="10" class="body"/>
    ${bar(48, 75, v(4), 'port 4')}${bar(212, 75, v(3), 'port 3')}${bar(48, 165, v(1), 'port 1')}${bar(212, 165, v(2), 'port 2')}
    <text class="tm" x="130" y="22" text-anchor="middle">bars: FORWARD up, BACKWARD down</text></svg>`;
}

route('demo', async () => {
  const s = { sketch: 'makeblock', stream: '$L200Z$R-90Z', gap: 0, fixed: true };
  app().innerHTML = `<p class="kicker">Interactive demo</p><h1>Serial command demo</h1>
  <p class="lead">The sketches' command reader and motor logic, ported line by line to JavaScript. In the tests the port
  produces exactly the same motor, servo and serial-echo calls as the real sketches compiled on a desktop with stand-in
  Arduino headers, for every byte stream in the test set. No hardware is simulated beyond what the code itself does.</p>
  <div class="demo-grid">
    <div class="panel">
      <label for="d-sk">Sketch</label>
      <select id="d-sk">${list(Object.entries(SKETCHES), ([k, v]) => `<option value="${k}">${esc(v.label)}</option>`)}</select>
      <label for="d-in">Bytes sent over serial</label><input id="d-in" value="${esc(s.stream)}" spellcheck="false" autocomplete="off">
      <div id="d-pre" class="small"></div>
      <label for="d-gap">Byte arrival: empty polls between bytes <b id="d-gapv">0</b></label>
      <input id="d-gap" type="range" min="0" max="6" value="0">
      <p class="small mute">0 = the whole command is already in the buffer. Higher values model the loop running ahead of
      bytes still arriving (Serial.read() then returns -1).</p>
      <div class="seg" role="group" aria-label="Reader version"><button id="d-fix" class="on" type="button">Current reader</button><button id="d-orig" type="button">Original reader (before the fix)</button></div>
    </div>
    <div class="panel"><div id="d-pic" class="pic"></div><div id="d-kv" class="kvs"></div></div>
  </div>
  <div class="two">
    <div class="panel"><h3>Commands handed to handle_command()</h3><div id="d-cmd" class="out"></div>
      <h3>Serial echo</h3><pre id="d-tx" class="out"></pre></div>
    <div class="panel"><h3>Driver calls (first 60, repeats per loop folded)</h3><pre id="d-ev" class="out log"></pre></div>
  </div>`;
  const $ = id => document.getElementById(id);
  const presets = () => { $('d-pre').innerHTML = 'Try: ' + list(PRESETS[s.sketch], ([p, t]) => `<button type="button" class="alt pre" data-p="${esc(p)}" title="${esc(t)}">${esc(p)}</button>`); };
  function draw() {
    const { events, state } = run(s.sketch, s.stream, { gap: s.gap, fixed: s.fixed });
    const out = finalOutputs(events);
    $('d-pic').innerHTML = wheels(s.sketch, out);
    const kv = [['left', state.left], ['right', state.right]];
    if (s.sketch === 'adafruit') kv.push(['bottom servo (pin 10)', out.S10], ['top servo (pin 9)', out.S9]);
    else kv.push(['M1 dir pin / PWM', `${makeblockPins(out.M1).dir} / ${makeblockPins(out.M1).pwm}`], ['M2 dir pin / PWM', `${makeblockPins(out.M2).dir} / ${makeblockPins(out.M2).pwm}`]);
    $('d-kv').innerHTML = list(kv, ([k, v]) => `<div><b>${esc(String(v))}</b><span>${esc(k)}</span></div>`);
    $('d-cmd').innerHTML = state.commands.length ? list(state.commands, c => `<code>"${esc(show(c))}"</code>`, ' ') +
      (state.commands.some(c => c.includes(255)) ? '<p class="small err">· = a -1 (0xFF) stored for a byte that had not arrived: the letter check fails or atoi stops early.</p>' : '')
      : '<span class="mute">none</span>';
    if (events.includes('HANG')) $('d-cmd').innerHTML += '<p class="small err">No closing Z: the reader keeps waiting (stopped here after 100000 polls).</p>';
    const tx = events.filter(e => e.startsWith('TX:')).map(e => e.slice(3).replace(/\\n/g, '\n')).join('');
    $('d-tx').textContent = tx || (s.sketch === 'adafruit' ? '(echo is commented out in this sketch)' : '');
    const folded = [];
    let prev = null;
    for (const e of events) {
      const key = e;
      if (key === '--- loop' && prev === '--- loop') continue;
      folded.push(key); prev = key;
    }
    const loops = [], cur = [];
    for (const e of folded) { if (e === '--- loop') { if (cur.length) loops.push(cur.join('\n')); cur.length = 0; } else cur.push(e); }
    if (cur.length) loops.push(cur.join('\n'));
    const lines = [];
    loops.forEach((l, i) => { if (i && l === loops[i - 1]) { if (!lines.at(-1).startsWith('  (same')) lines.push('  (same calls repeat every loop)'); } else lines.push(...l.split('\n')); });
    $('d-ev').textContent = lines.slice(0, 60).join('\n') + (lines.length > 60 ? '\n…' : '');
  }
  $('d-sk').addEventListener('change', e => { s.sketch = e.target.value; presets(); draw(); });
  $('d-in').addEventListener('input', e => { s.stream = e.target.value; draw(); });
  $('d-gap').addEventListener('input', e => { s.gap = +e.target.value; $('d-gapv').textContent = s.gap; draw(); });
  $('d-pre').addEventListener('click', e => { const b = e.target.closest('[data-p]'); if (b) { s.stream = $('d-in').value = b.dataset.p; draw(); } });
  $('d-fix').addEventListener('click', () => { s.fixed = true; $('d-fix').classList.add('on'); $('d-orig').classList.remove('on'); draw(); });
  $('d-orig').addEventListener('click', () => { s.fixed = false; $('d-orig').classList.add('on'); $('d-fix').classList.remove('on'); draw(); });
  presets();
  draw();
});
