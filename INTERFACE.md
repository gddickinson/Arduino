# INTERFACE.md - navigation map

## Sketches (Arduino IDE, one folder per sketch)
- `Arduino_serial_control_WORKS/` - `$L<n>Z`/`$R<n>Z` serial control of two Makeblock DC motors (clamp ±255, deadband 5).
- `Arduino_serial_control_Adafruit_WORKS/` - same protocol for 4 DC motors on an Adafruit Motor Shield v2 plus `$B`/`$T` servos.
- `Robot_Base_Code4/` - Makeblock robot base: IR remote driving, camera servos, serial mode; split over several `.ino` tabs.
- `Robot_Base_Code`..`3/` - earlier versions (start from Makeblock's DualMode_Starter demo).
- `piRobot_motorControl/`, `drawingRobot_motorControl/`, `test_2/` - further serial-control variants.
- Sensors: `NewPing05Sensors/`, `NewPing07Sensors/`, `NewPingExample/`, `Ping05Sensors_python/`, `sonar_test*`,
  `drawingRobot_sonor/` (ultrasonic); `LSM303_calibration/`; `WheelSpeed*/`, `wheel_speed_function/`, `encoder_interrupts/`.
- Other: `terrarium_main/` (DHT22 + LCD + fan), `clockRadio/` (I2C LCD), `ir_remote*/`, `TestInfraredReceiver/`,
  `XV_Lidar_Controller/` (third-party), motor/servo tests, `pyserial_test*/`.
- `libraries/` - vendored third-party Arduino libraries (not this project's code; licences per folder).

## Web interface (`web/`)
- `web/app.py` - FastAPI app: `/api/info`, `/tree`, `/file/{id}`, `/corpus`, `/vendored`, `/how`, `/about`; `STATIC_ENDPOINTS`.
- `web/codebase.py` - repo scanner: shown/skipped files, language, header-comment summaries, vendored licence detection.
- `web/project.py` - this repo's text: title, excludes, vendored folders, How-it-works sections with citations, About.
- `web/static/` - front end: `main.js` (router setup), `home.js` (home + about), `code.js` (explorer), `highlight.js`
  (+ `.test.mjs`), `how.js`, `demo.js`, `serial_engine.js` (JS port of the serial sketches, + `.test.mjs`), `core.js`,
  `static_api.js`, `style.css`, `style_app.css`.
- `run_web.py` - starts the app on 127.0.0.1:8258. `scripts/build_static.py` - GitHub Pages export.
  `scripts/screenshots.py` - smoke test + README images.

## Tests and the C++ harness
- `tests/arduino_harness/` - stand-in `Arduino.h`, `Makeblock.h`, `Adafruit_MotorShield.h`, `Servo.h` and `harness.cpp`
  to compile a serial sketch on a desktop and replay a byte stream (logs every driver call).
- `scripts/serial_harness.py` - builds/runs the harness ("fixed" = repo sketch, "original" = without the added wait).
- `scripts/make_serial_fixture.py` -> `tests/fixtures/serial_cases.json` (used by `web/static/serial_engine.test.mjs`).
- `tests/test_serial_sketches.py` - regression tests for the reader fix. `tests/test_web.py` - API, static build,
  citations, vendored licences, fixture currency.
