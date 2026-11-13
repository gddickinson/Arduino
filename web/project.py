"""Project-specific content for the Arduino sketches showcase: what the code explorer skips, the vendored library
folders, the home-page tiles, the "How it works" sections (every claim cited to a file + a text that must occur on the
cited line; the server resolves the line number, and tests fail if a citation no longer matches) and About.
"""
from fastapi import FastAPI

TITLE = "Arduino Robot Sketches"
REPO = "Arduino"
PORT = 8258
KICKER = "Arduino · C++ · mobile robots"
DESCRIPTION = ("Arduino sketches for mobile robots - serial control, DC motor and servo drivers, LSM303 compass "
               "calibration, HC-SR04 ultrasonic sensor arrays, wheel-speed encoders, and a terrarium controller.")
DEMO = {"title": "Serial command demo",
        "blurb": "Type $L200Z-style commands and watch the real parser and motor logic (ported to JS, checked against "
                 "the compiled sketches) drive the motors and servos."}

# Web-interface files and generated/OS files are not part of the project's code.
EXCLUDE = ("web/*", "scripts/*", "tests/*", "run_web.py", "requirements-web.txt", ".github/*", "docs/*",
           "SESSION_LOG.md", "INTERFACE.md", ".DS_Store", "*/.DS_Store", ".gitignore",
           "libraries/readme.txt")
VENDORED = tuple((f"libraries/{n}", n) for n in (
    "AccelStepper-master", "Adafruit_GPS", "Adafruit_GPS_Library", "Adafruit_HMC5883", "Adafruit_LSM303DLHC",
    "Adafruit_Motor_Shield_V2_Library", "Adafruit_Sensor", "AFMotor", "Arduino_IRremote", "DHT2pin", "DHTlib",
    "DHTstable", "Firmata", "IRremote", "Kalman_Filter_Library", "LIDAR-Lite_v3", "LSM303", "makeblock", "NewPing",
    "NewTone", "PID", "Robot_Control", "RobotMotor", "ros_lib", "Sainsmart_HMC5883L", "SD", "SerialCommand",
    "SparkFun_LSM303C_6_DOF_IMU_Breakout", "Temboo", "TimerThree"))
NOT_CODE = ("Markdown", "Text")


def tiles(shown):
    sketches = {f["path"].rsplit("/", 1)[0] for f in shown if f["path"].endswith(".ino") and "/" in f["path"]}
    return [[len(sketches), "sketch folders"], [sum(f["path"].endswith(".ino") for f in shown), ".ino files"]]


SERIAL = "Arduino_serial_control_WORKS/Arduino_serial_control_WORKS.ino"
ADA = "Arduino_serial_control_Adafruit_WORKS/Arduino_serial_control_Adafruit_WORKS.ino"
RB4 = "Robot_Base_Code4/Robot_Base_Code4.ino"

SVG_SERIAL = """<svg viewBox="0 0 760 210" role="img" aria-label="Data flow: a computer sends $L and $R commands over serial at
19200 baud; the sketch frames them between $ and Z, parses the number with atoi, clamps it, applies a deadband and
drives two motors">
<defs><marker id="arw" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto">
<path class="arh" d="M0 0L10 5L0 10z"/></marker></defs>
<rect class="bx3" x="10" y="70" width="140" height="64" rx="6"/><text class="t" x="80" y="98" text-anchor="middle">PC / Pi</text>
<text class="tm" x="80" y="118" text-anchor="middle">"$L200Z$R-90Z"</text>
<path class="ar" d="M150 102H205"/><text class="tm" x="178" y="92" text-anchor="middle">serial</text>
<text class="tm" x="178" y="124" text-anchor="middle">19200</text>
<rect class="bx" x="208" y="40" width="170" height="124" rx="6"/><text class="t" x="293" y="64" text-anchor="middle">loop(): reader</text>
<text class="tm" x="293" y="88" text-anchor="middle">'$' starts a command</text><text class="tm" x="293" y="106" text-anchor="middle">bytes until 'Z'</text>
<text class="tm" x="293" y="124" text-anchor="middle">handle_command()</text><text class="tm" x="293" y="142" text-anchor="middle">L/R + atoi(rest)</text>
<path class="ar" d="M378 102H430"/>
<rect class="bx2" x="433" y="40" width="150" height="124" rx="6"/><text class="t" x="508" y="64" text-anchor="middle">motor logic</text>
<text class="tm" x="508" y="88" text-anchor="middle">clamp to ±255</text><text class="tm" x="508" y="106" text-anchor="middle">|v| ≤ 5 → stop()</text>
<text class="tm" x="508" y="124" text-anchor="middle">else run(v)</text>
<path class="ar" d="M583 80H640"/><path class="ar" d="M583 124H640"/>
<rect class="bx3" x="643" y="56" width="105" height="40" rx="6"/><text class="t" x="695" y="81" text-anchor="middle">motor1 (M1)</text>
<rect class="bx3" x="643" y="106" width="105" height="40" rx="6"/><text class="t" x="695" y="131" text-anchor="middle">motor2 (M2)</text>
</svg>"""

HOW = [
    {"id": "protocol", "title": "The serial command protocol", "svg": SVG_SERIAL,
     "caption": "Arduino_serial_control_WORKS: from a command string to the two motors.",
     "body": """<p>Several sketches share one text protocol, stated in their header: a command is a letter and a number
framed by <code>$</code> and <code>Z</code>, e.g. <code>$L255Z</code> (left motor forward 255) and <code>$R255Z</code>.
The serial port runs at 19200 baud. In <code>loop()</code>, a <code>$</code> starts a new command; every byte up to the
<code>Z</code> is appended to a <code>String</code>, which is then passed to <code>handle_command()</code>.</p>
<p><code>handle_command()</code> offers the string to one setter per letter. Each setter checks the first character and
converts the rest with <code>atoi</code> (after copying at most 18 characters into a 20-byte buffer), so
<code>$L-90Z</code> sets <code>left = -90</code> and anything that is not a number reads as 0. The 2-motor sketch echoes
the values back (<code>left: … right: …</code>); the Adafruit version has the echo commented out. Letters the sketch
does not know are ignored.</p>""",
     "cites": [
         {"file": SERIAL, "find": "$L255Z", "note": "the protocol, in the sketch's header"},
         {"file": SERIAL, "find": "Serial.begin(19200)", "note": "baud rate"},
         {"file": SERIAL, "find": "char command_begin = '$';"},
         {"file": SERIAL, "find": "char command_end = 'Z';"},
         {"file": SERIAL, "find": "readString += current_char;", "note": "bytes collected until Z"},
         {"file": SERIAL, "find": "the_string.substring(1).toCharArray(temp, 19);", "note": "at most 18 chars copied"},
         {"file": SERIAL, "find": "int l_val = atoi(temp);"},
         {"file": SERIAL, "find": "Serial.print(\"left: \");", "note": "echo of the parsed values"},
     ]},
    {"id": "motors", "title": "From a number to a motor (Makeblock, 2 motors)",
     "body": """<p>After reading, the sketch clamps <code>left</code> and <code>right</code> to -255…255 (they become PWM
duty values) and applies a <b>deadband</b> of 5: a value between -5 and 5 stops the motor so that a jittery signal near
zero does not creep the robot; anything larger is passed to <code>MeDCMotor::run()</code> with its sign. The Makeblock
library's <code>run()</code> (in <code>libraries/makeblock/Makeblock.cpp</code>, vendored) clamps again, sets the
direction pin HIGH for a value ≥ 0 and LOW otherwise, and writes the magnitude as PWM; <code>stop()</code> is
<code>run(0)</code>. Because the clamp overwrites the global, a command such as <code>$L900Z</code> is echoed as 900 and
then held as 255.</p>""",
     "cites": [
         {"file": SERIAL, "find": "int deadband = 5;"},
         {"file": SERIAL, "find": "if(left > 255){"},
         {"file": SERIAL, "find": "if(left > deadband){"},
         {"file": SERIAL, "find": "motor1.stop();", "note": "inside the deadband (also at start-up)"},
         {"file": SERIAL, "find": "MeDCMotor motor1(M1);"},
     ]},
    {"id": "adafruit", "title": "Four motors and two servos (Adafruit Motor Shield v2)",
     "body": """<p>The Adafruit variant adds two servo commands, <code>$B…Z</code> (bottom) and <code>$T…Z</code> (top), on
pins 10 and 9, both starting at 75 and limited to 5…175 before <code>servo.write()</code>. Every pass of
<code>loop()</code> sends the left value to <code>motorRun2</code> and <code>motorRun4</code> and the right value to
<code>motorRun1</code> and <code>motorRun3</code>, each clamped to ±255. <code>motorRun1/2</code> run BACKWARD for a
value ≥ 0 while <code>motorRun3/4</code> run FORWARD, which compensates for motors mounted facing the other way; the
magnitude goes to <code>setSpeed()</code>. There is no deadband in this version. The loop waits 25 ms after each of
the six outputs, so one pass takes about 150 ms plus two more 25 ms waits inside the servo functions.</p>
<p>One thing worth knowing when wiring: <code>myMotor1</code>…<code>myMotor4</code> are created on shield ports
<code>frontLeft = 4</code>, <code>frontRight = 3</code>, <code>backLeft = 1</code>, <code>backRight = 2</code>, so the
"left" value drives the motors on the ports named frontRight and backRight. The demo shows the shield port each
command reaches.</p>""",
     "cites": [
         {"file": ADA, "find": "int frontLeft = 4;"},
         {"file": ADA, "find": "Adafruit_DCMotor *myMotor1 = AFMS.getMotor(frontLeft);"},
         {"file": ADA, "find": "motorSetSpeed(left,2);"},
         {"file": ADA, "find": "motorSetSpeed(right,1);"},
         {"file": ADA, "find": "myMotor1->run(BACKWARD);"},
         {"file": ADA, "find": "myMotor3->run(FORWARD);"},
         {"file": ADA, "find": "if (b_pos >175) {b_pos = 175;}"},
         {"file": ADA, "find": "servo1.attach(10);"},
     ]},
    {"id": "reader-fix", "title": "A fix: waiting for bytes that have not arrived",
     "body": """<p>The command reader used to call <code>Serial.read()</code> for each byte of a command without checking
<code>Serial.available()</code>. <code>Serial.read()</code> returns -1 when nothing has arrived yet, so if the loop got
ahead of the incoming bytes (each byte takes about 0.5 ms at 19200 baud) the command filled up with 0xFF characters, the
letter check failed and the command was ignored. Each copy of the reader now waits for the next byte with one added
line. When the whole command is already in the buffer nothing changes. The demo can replay both versions; the
behaviour was confirmed by compiling the real sketches on a desktop with stand-in Arduino headers
(<code>tests/arduino_harness</code>).</p>""",
     "cites": [
         {"file": SERIAL, "find": "while (!Serial.available()) {}", "note": "the added wait"},
         {"file": ADA, "find": "while (!Serial.available()) {}"},
         {"file": RB4, "find": "while (!Serial.available()) {}"},
     ]},
    {"id": "ir-robot", "title": "Robot_Base_Code4: IR remote, camera servos and serial mode",
     "body": """<p>The Makeblock robot base reacts to an infrared remote: PLUS/MINUS drive forward/backward,
NEXT/PREVIOUS turn, number keys set the speed to <code>factor*n + minSpeed</code> (23·n + 48, so key 9 gives 255), and
TEST/RETURN/POWER/MENU/0 move the pan-tilt camera servos in 8° steps or re-centre them. PLAY toggles the mode: in mode
0 the robot stops when no key is held; in mode 1 it reads serial commands with the same <code>$L…Z</code> protocol,
only when <code>millis() % 50 == 0</code>. The sketch is split over several <code>.ino</code> tabs (motor, servo,
sonar, gyro and wheel-speed functions), which the Arduino IDE concatenates.</p>""",
     "cites": [
         {"file": RB4, "find": "case IR_BUTTON_PLUS:"},
         {"file": RB4, "find": "ChangeSpeed(factor*9+minSpeed);"},
         {"file": RB4, "find": "int minSpeed = 48;"},
         {"file": RB4, "find": "int factor = 23;"},
         {"file": RB4, "find": "if(++mode==2) mode=0;"},
         {"file": RB4, "find": "else if(millis()%50 == 0)"},
         {"file": "Robot_Base_Code4/Servo_Functions.ino", "find": "camHorizontal = camHorizontal -8;"},
         {"file": "Robot_Base_Code4/Motor_Functions.ino", "find": "void TurnLeft()"},
     ]},
    {"id": "sensors", "title": "Sensors: ultrasonic arrays, compass calibration, wheel encoders, terrarium",
     "body": """<p><b>Ultrasonic arrays</b> (NewPing05/07Sensors, Ping05Sensors_python) are based on the NewPing library's
15-sensor example: each HC-SR04 is pinged in turn every 33 ms from a timer, and a full cycle hands all distances to
<code>oneSensorCycle()</code>. Ping05Sensors_python notes that NewPing.h was adjusted to report millimetres and prints
the readings labelled by sensor letter, e.g. <code>(A…)</code>.</p>
<p><b>LSM303 calibration</b> tracks the minimum and maximum of each accelerometer and magnetometer axis and prints them
once a second, the numbers needed to offset and scale a compass.</p>
<p><b>Wheel encoders</b>: Robot_Base_Code4's <code>Wheel_Speed()</code> counts changes of an analogue encoder signal
(above/below 500) and converts counts to distance with <code>distanceTooth = 2πr / teeth</code>.</p>
<p><b>Terrarium controller</b>: a DHT22 temperature/humidity sensor on pin 5, a 20×4 I²C LCD, a clock, two
potentiometers mapped to temperature (5…99) and humidity (0…99) set-points, and a fan output on pin 8.</p>""",
     "cites": [
         {"file": "NewPing05Sensors/NewPing05Sensors.ino", "find": "#define PING_INTERVAL 33"},
         {"file": "NewPing05Sensors/NewPing05Sensors.ino", "find": "oneSensorCycle(); // Sensor ping cycle complete"},
         {"file": "Ping05Sensors_python/Ping05Sensors_python.ino", "find": "adjusted NewPing.h code to get mm"},
         {"file": "LSM303_calibration/LSM303_calibration.ino", "find": "if (magEvent.magnetic.x < MagMinX)"},
         {"file": "Robot_Base_Code4/WheelSpeed.ino", "find": "if (newWheelState> halfState){"},
         {"file": RB4, "find": "int distanceTooth = ((2*PI)*wheelRadius)/wheelTeeth;"},
         {"file": "terrarium_main/terrarium_main.ino", "find": "#define DHT22_PIN 5"},
         {"file": "terrarium_main/terrarium_main.ino", "find": "temp_set = map(temp_set, 0, 1023, t_min , t_max);"},
         {"file": "terrarium_main/terrarium_main.ino", "find": "const int fanPin1 = 8;"},
     ]},
]

ABOUT = {
    "port": PORT,
    "what": """<p>A collection of Arduino sketches written while building small mobile robots (2017, with a library
update in 2021): serial-controlled motor drivers for a Makeblock base and an Adafruit Motor Shield v2 robot, an
IR-remote robot base with a pan-tilt camera, ultrasonic sensor arrays, compass calibration, wheel encoders, IR remote
tests, an I²C LCD clock and a terrarium controller. The serial sketches are the Arduino end of the ROS robot in
<a href="https://github.com/gddickinson/robot_code">robot_code</a>, whose Python nodes send the same
<code>$L…Z</code>/<code>$R…Z</code> commands.</p>""",
    "needs": ["An Arduino-compatible board (the Makeblock sketches target a Makeblock baseboard or base shield, the others an "
              "Arduino with the Adafruit Motor Shield v2 or the sensors named in each sketch).",
              "The Arduino IDE, with this repo's <code>libraries/</code> folder as the sketchbook libraries.",
              "For serial control: a computer or Raspberry Pi sending commands at 19200 baud."],
    "run": ["Open a sketch folder in the Arduino IDE, select the board and port, and upload.",
            "Open the Serial Monitor at the sketch's baud rate (19200 for the serial-control sketches) and send e.g. "
            "<code>$L150Z</code>."],
    "credits": ["Sketches by George Dickinson, several starting from library examples: Makeblock's DualMode_Starter "
                "demo (Robot_Base_Code 1-3, CC-BY-SA 3.0 per its header), the NewPing 15-sensor example, Adafruit "
                "Motor Shield v2 test sketches, Arduino's Sweep example, and the XV Lidar Controller by James LeRoy "
                "(getSurreal), each credited in the file's header.",
                "Third-party libraries in <code>libraries/</code>: see the table below."],
    "licence": "MIT (see LICENSE) for George's own code; THIRD_PARTY.md notes that everything under "
               "<code>libraries/</code> is under each library's own licence.",
}

EXTRA_ENDPOINTS = []


def register(app: FastAPI):
    """No extra routes: the demo runs entirely in the browser (web/static/serial_engine.js)."""
