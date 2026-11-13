// JS port of the serial command parser and motor/servo logic of two sketches:
//   Arduino_serial_control_WORKS          (Makeblock board, 2 DC motors: $L<n>Z, $R<n>Z)
//   Arduino_serial_control_Adafruit_WORKS (Adafruit Motor Shield v2, 4 DC motors + 2 servos: $L $R $B $T)
// run(sketch, stream, {gap, fixed}) replays setup() and loop() exactly as the C++ does and returns the same event lines
// as the desktop harness in tests/arduino_harness (compared in serial_engine.test.mjs against the compiled originals).
// `gap` models bytes still arriving: after each byte the next one is missing for `gap` polls, during which
// Serial.available() is 0 and Serial.read() returns -1. `fixed` adds the one-line wait the sketches now have
// (`while (!Serial.available()) {}` before each read inside a command); without it the original reads -1 (0xFF).

export const SKETCHES = {
  makeblock: { file: 'Arduino_serial_control_WORKS/Arduino_serial_control_WORKS.ino', label: 'Makeblock, 2 motors (Arduino_serial_control_WORKS)' },
  adafruit: { file: 'Arduino_serial_control_Adafruit_WORKS/Arduino_serial_control_Adafruit_WORKS.ino', label: 'Adafruit Motor Shield v2, 4 motors + 2 servos (…_Adafruit_WORKS)' },
};

// C atoi on the chars the sketch copies: toCharArray(temp, 19) keeps at most 18 chars.
export function atoi(chars) {
  let i = 0, sign = 1, v = 0;
  const s = chars.slice(0, 18);
  while (i < s.length && ' \t\n\v\f\r'.includes(String.fromCharCode(s[i]))) i++;
  if (s[i] === 43 || s[i] === 45) { sign = s[i] === 45 ? -1 : 1; i++; }
  while (i < s.length && s[i] >= 48 && s[i] <= 57) { v = v * 10 + (s[i] - 48); i++; }
  return sign * v;
}

class SerialIn {
  constructor(bytes, gap) { this.data = bytes; this.pos = 0; this.gap = gap; this.wait = 0; this.idle = 0; }
  more() { return this.pos < this.data.length; }
  poll() { if (!this.more()) { if (++this.idle > 100000) throw new Error('HANG'); return false; } if (this.wait > 0) { this.wait--; return false; } return true; }
  available() { return this.poll() ? 1 : 0; }
  read() { if (!this.poll()) return -1; this.wait = this.gap; return this.data[this.pos++]; }
}

const toChar = b => (b << 24) >> 24;   // int -> signed char, as `char current_char = Serial.read()` does
const DIRS = ['?', 'FORWARD', 'BACKWARD', 'BRAKE', 'RELEASE'];

export function run(sketch, stream, { gap = 0, fixed = true, maxLoops = 100000 } = {}) {
  const ev = [];
  const bytes = typeof stream === 'string' ? [...stream].map(c => c.charCodeAt(0) & 0xff) : stream;
  const ser = new SerialIn(bytes, gap);
  const st = { left: 0, right: 0, top: 75, bottom: 75, commands: [] };
  const tx = t => ev.push('TX:' + t);
  const mb = (port, speed) => ev.push(speed === null ? `M${port}.stop()` : `M${port}.run(${speed})`);
  // Adafruit: myMotor1..4 = getMotor(frontLeft=4), getMotor(frontRight=3), getMotor(backLeft=1), getMotor(backRight=2)
  const PORT = [0, 4, 3, 1, 2];
  const af = (k, dir, speed) => { ev.push(`AF${PORT[k]}.run(${DIRS[dir]})`); if (speed !== undefined) ev.push(`AF${PORT[k]}.setSpeed(${speed})`); };
  const servo = (pin, a) => ev.push(`S${pin}.write(${a})`);

  const value = (cmd, letter) => cmd.length && cmd[0] === letter.charCodeAt(0) ? atoi(cmd.slice(1)) : null;
  function handle(cmd) {
    st.commands.push(cmd.slice());
    const l = value(cmd, 'L'), r = value(cmd, 'R');
    if (l !== null) st.left = l;
    if (r !== null) st.right = r;
    if (sketch === 'adafruit') {
      const b = value(cmd, 'B'), t = value(cmd, 'T');
      if (b !== null) st.bottom = b;
      if (t !== null) st.top = t;
    } else {
      tx('left: '); tx(String(st.left)); tx('     '); tx('right: '); tx(String(st.right)); tx('     \\n');
    }
  }
  function readSerial() {
    while (ser.available()) {
      let c = toChar(ser.read());
      if (c === 36) {                                  // '$'
        let cmd = [];
        while (c !== 90) {                             // 'Z'
          if (fixed) while (!ser.available()) { /* wait for the byte */ }
          c = toChar(ser.read());
          if (c !== 90) cmd.push(c & 0xff);
        }
        handle(cmd);
      }
    }
  }
  const clamp = v => Math.max(-255, Math.min(255, v));
  function motorRun(k, s) {                         // motorRun1/2 are wired reversed relative to motorRun3/4
    const rev = k === 1 || k === 2;
    if (s >= 0) af(k, rev ? 2 : 1, s); else af(k, rev ? 1 : 2, Math.abs(s));
  }
  function loop() {
    readSerial();
    if (sketch === 'adafruit') {
      motorRun(2, clamp(st.left)); motorRun(4, clamp(st.left));
      motorRun(1, clamp(st.right)); motorRun(3, clamp(st.right));
      servo(10, Math.max(5, Math.min(175, st.bottom)));
      servo(9, Math.max(5, Math.min(175, st.top)));
    } else {
      st.left = clamp(st.left); st.right = clamp(st.right);
      mb(1, st.left > 5 || st.left < -5 ? st.left : null);
      mb(2, st.right > 5 || st.right < -5 ? st.right : null);
    }
  }
  // setup()
  if (sketch === 'adafruit') { [1, 2, 3, 4].forEach(k => af(k, 4)); servo(10, 75); servo(9, 75); }
  else { mb(1, null); mb(2, null); }
  ev.push('--- loop');
  try {
    for (let i = 0; i < maxLoops && ser.more(); i++) { loop(); ev.push('--- loop'); }
    loop();
  } catch (e) {
    if (e.message !== 'HANG') throw e;
    ev.push('HANG');
  }
  return { events: ev, state: st };
}

// What each motor driver does with a run(speed) call, from the libraries in the repo:
// Makeblock MeDCMotor::run (libraries/makeblock/Makeblock.cpp) clamps to +-255, sets the direction pin HIGH for
// speed >= 0 (LOW otherwise) and writes |speed| as PWM; stop() is run(0).
export function makeblockPins(speed) {
  const s = Math.max(-255, Math.min(255, speed ?? 0));
  return { dir: s >= 0 ? 'HIGH' : 'LOW', pwm: Math.abs(s) };
}

// Final output of each actuator after a run: last value written per motor/servo.
export function finalOutputs(events) {
  const out = {};
  for (const e of events) {
    let m;
    if ((m = e.match(/^M(\d)\.run\((-?\d+)\)$/))) out['M' + m[1]] = +m[2];
    else if ((m = e.match(/^M(\d)\.stop\(\)$/))) out['M' + m[1]] = 0;
    else if ((m = e.match(/^AF(\d)\.run\((\w+)\)$/))) out['AF' + m[1]] = { ...(out['AF' + m[1]] || {}), dir: m[2] };
    else if ((m = e.match(/^AF(\d)\.setSpeed\((\d+)\)$/))) out['AF' + m[1]] = { ...(out['AF' + m[1]] || {}), speed: +m[2] };
    else if ((m = e.match(/^S(\d+)\.write\((\d+)\)$/))) out['S' + m[1]] = +m[2];
  }
  return out;
}
