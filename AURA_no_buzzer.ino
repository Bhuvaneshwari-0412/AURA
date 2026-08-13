/*
  =========================================================
  AURA — Room Occupancy & Mood Light Monitor
  USB-Powered Edition (NO buzzer, 4 separate mood LEDs +
  1 direct-driven activity LED, cloned MPU-6050 support)
  =========================================================

  Changes from the previous version:
    - Buzzer removed completely (pin, wiring calls, logic).
    - LEDs already used as 4 separate LEDs (R/G/Y/B) - kept as is.
    - MPU-6050 is now read with RAW I2C register access instead of
      the Adafruit_MPU6050 library. Many cloned MPU-6050 boards
      report a wrong/blank WHO_AM_I value, which makes
      Adafruit_MPU6050's mpu.begin() fail even though the sensor
      itself works fine. Talking to it directly at the register
      level avoids that check entirely.

  Pin map:
    D2  - Ultrasonic 1 (Entry)  TRIG
    D3  - Ultrasonic 1 (Entry)  ECHO
    D4  - Ultrasonic 2 (Exit)   TRIG
    D5  - Ultrasonic 2 (Exit)   ECHO
    D6  - Red LED    (via 220R)
    D7  - Green LED  (via 220R)
    D8  - Yellow LED (via 220R)
    D9  - Blue LED   (via 220R)
    D10 - HC-05 TXD -> Arduino RX   (SoftwareSerial RX)
    D11 - HC-05 RXD <- Arduino TX, via 1k resistor (SoftwareSerial TX)
    D12 - Activity/Vibration LED, direct-driven from the pin
          (no transistor needed for a single LED):
          D12 -> 220R resistor -> LED anode -> LED cathode -> GND
          ON continuously while peopleCount > 0 (stays on the whole
          time anyone is in the room, turns OFF only when the count
          returns to 0). Independently, also gives a brief ON blip
          when the MPU-6050 detects a tap/vibration while the room
          is empty.
    A0  - Button 1 (increase capacity), INPUT_PULLUP
    A1  - Button 2 (decrease capacity), INPUT_PULLUP
    A4  - I2C SDA (MPU-6050 + OLED, shared bus)
    A5  - I2C SCL (MPU-6050 + OLED, shared bus)

  Required libraries (install via Library Manager):
    - Adafruit GFX Library
    - Adafruit SSD1306
  (SoftwareSerial and Wire are built in. MPU-6050 no longer
   needs a library — it's read directly over I2C.)

  =========================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

// ---------------- Pin definitions ----------------
#define TRIG1 2
#define ECHO1 3
#define TRIG2 4
#define ECHO2 5

#define LED_RED    6
#define LED_GREEN  7
#define LED_YELLOW 8
#define LED_BLUE   9

#define HC05_RX 10   // Arduino RX  <- HC-05 TXD
#define HC05_TX 11   // Arduino TX  -> HC-05 RXD (through 1k resistor)

#define BTN_INC A0
#define BTN_DEC A1

#define ACTIVITY_LED 12   // direct-driven LED: ON on tap/motion, OFF when calm

// ---------------- MPU-6050 raw I2C setup ----------------
#define MPU_ADDR        0x68  // AD0 tied to GND -> 0x68
#define MPU_REG_PWR_MGMT_1  0x6B
#define MPU_REG_ACCEL_XOUT_H 0x3B
bool mpuFound = false;

// ---------------- Objects ----------------
SoftwareSerial btSerial(HC05_RX, HC05_TX); // RX, TX
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------------- State ----------------
int peopleCount   = 0;
int capacityLimit = 10;      // adjustable with buttons, min 1
const int CAPACITY_MIN = 1;
const int CAPACITY_MAX = 50;

// entry/exit sequence detection
const int TRIGGER_DIST_CM   = 15;   // distance under which a sensor counts as "triggered"
const unsigned long SEQUENCE_WINDOW_MS = 1200; // time allowed between sensor1 and sensor2 trigger

unsigned long sensor1TriggerTime = 0;
unsigned long sensor2TriggerTime = 0;
bool sensor1Armed = true;
bool sensor2Armed = true;

// button debounce
unsigned long lastIncPress = 0;
unsigned long lastDecPress = 0;
const unsigned long DEBOUNCE_MS = 250;

// bluetooth periodic update
unsigned long lastBtSend = 0;
const unsigned long BT_INTERVAL_MS = 1000;

// MPU vibration (values in g's; gravity alone ~1.0g on a perfect sensor,
// but cheap clones often have an offset - so we calibrate the real
// resting value at boot instead of assuming exactly 1.0g)
const float VIBRATION_THRESHOLD_G = 0.3;
float mpuRestBaselineG = 1.0; // overwritten by calibrateMpuBaseline() in setup()
unsigned long lastVibrationShown = 0;

// Activity LED (D12, via transistor) - stays ON for a short hold time
// after a tap/vibration or close approach, so it reads as a clean
// on/off state rather than flickering for an instant.
const int PROXIMITY_ACTIVITY_CM = 30;      // "someone near the sensor" distance
const unsigned long ACTIVITY_HOLD_MS = 1500; // how long the LED stays on after activity
unsigned long activityLedOffTime = 0;
bool activityLedOn = false;

// ---------------- Function prototypes ----------------
long getDistanceCM(int trigPin, int echoPin);
void updateLEDs();
void handleButtons();
void handleUltrasonics();
void updateDisplay(float vibeMag, bool vibrationDetected, bool mpuOk);
void sendBluetoothStatus();
void mpuInit();
bool mpuReadAccelG(float &magG);
void calibrateMpuBaseline();
void triggerActivityLed();
void updateActivityLed();

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(BTN_INC, INPUT_PULLUP);
  pinMode(BTN_DEC, INPUT_PULLUP);

  pinMode(ACTIVITY_LED, OUTPUT);
  digitalWrite(ACTIVITY_LED, LOW); // starts OFF - calm/quiet

  Wire.begin();

  // OLED init (0x3C is the typical SSD1306 I2C address)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found! Check wiring/address."));
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("AURA booting..."));
    display.display();
  }

  mpuInit();
  if (mpuFound) {
    calibrateMpuBaseline();
  }

  Serial.println(F("AURA ready."));
  updateLEDs();
  delay(500);
}

void loop() {
  handleButtons();
  handleUltrasonics();
  updateLEDs();

  float mag = 0;
  bool vibrationDetected = false;
  bool mpuOk = mpuReadAccelG(mag);
  if (mpuOk && fabs(mag - mpuRestBaselineG) > VIBRATION_THRESHOLD_G) {
    vibrationDetected = true;
    triggerActivityLed();
  }

  updateActivityLed();
  updateDisplay(mag, vibrationDetected, mpuOk);

  if (millis() - lastBtSend > BT_INTERVAL_MS) {
    sendBluetoothStatus();
    lastBtSend = millis();
  }

  delay(50); // small loop delay, keeps ultrasonic pulses stable
}

// ---------------- Ultrasonic distance ----------------
long getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout ~ 3.4m max
  if (duration == 0) return -1; // no echo / out of range
  long distanceCM = duration * 0.0343 / 2;
  return distanceCM;
}

// ---------------- Entry/Exit logic ----------------
void handleUltrasonics() {
  long d1 = getDistanceCM(TRIG1, ECHO1);
  long d2 = getDistanceCM(TRIG2, ECHO2);
  unsigned long now = millis();

  bool s1Triggered = (d1 > 0 && d1 < TRIGGER_DIST_CM);
  bool s2Triggered = (d2 > 0 && d2 < TRIGGER_DIST_CM);

  // Someone standing close to either sensor counts as "activity",
  // separate from the stricter entry/exit sequence logic below.
  if ((d1 > 0 && d1 < PROXIMITY_ACTIVITY_CM) ||
      (d2 > 0 && d2 < PROXIMITY_ACTIVITY_CM)) {
    triggerActivityLed();
  }

  // Sensor 1 (entry side) triggers first
  if (s1Triggered && sensor1Armed) {
    sensor1TriggerTime = now;
    sensor1Armed = false;
    // if sensor2 already triggered recently -> this completes an EXIT sequence
    if (!sensor2Armed && (now - sensor2TriggerTime) < SEQUENCE_WINDOW_MS) {
      if (peopleCount > 0) peopleCount--;
      Serial.println(F("EXIT detected"));
      sensor2Armed = true;
    }
  }
  if (!s1Triggered) sensor1Armed = true;

  // Sensor 2 (exit side) triggers first
  if (s2Triggered && sensor2Armed) {
    sensor2TriggerTime = now;
    sensor2Armed = false;
    // if sensor1 already triggered recently -> this completes an ENTRY sequence
    if (!sensor1Armed && (now - sensor1TriggerTime) < SEQUENCE_WINDOW_MS) {
      if (peopleCount < capacityLimit) {
        peopleCount++;
        Serial.println(F("ENTRY detected"));
      } else {
        Serial.println(F("ENTRY blocked - at capacity"));
      }
      sensor1Armed = true;
    }
  }
  if (!s2Triggered) sensor2Armed = true;
}

// ---------------- Buttons ----------------
void handleButtons() {
  unsigned long now = millis();

  if (digitalRead(BTN_INC) == LOW && (now - lastIncPress) > DEBOUNCE_MS) {
    if (capacityLimit < CAPACITY_MAX) capacityLimit++;
    lastIncPress = now;
    Serial.print(F("Capacity increased to "));
    Serial.println(capacityLimit);
  }

  if (digitalRead(BTN_DEC) == LOW && (now - lastDecPress) > DEBOUNCE_MS) {
    if (capacityLimit > CAPACITY_MIN) capacityLimit--;
    lastDecPress = now;
    Serial.print(F("Capacity decreased to "));
    Serial.println(capacityLimit);
    if (peopleCount > capacityLimit) peopleCount = capacityLimit;
  }
}

// ---------------- Mood LEDs ----------------
// Matches the color chart in the wiring guide:
//   CALM (empty)          -> Green + Blue (cyan)
//   SETTLING (few)         -> Green only
//   MODERATE (half)        -> Yellow only
//   ENERGETIC (near full)  -> Yellow + Red (orange)
//   FULL                   -> Red only
void updateLEDs() {
  float ratio = capacityLimit > 0 ? (float)peopleCount / capacityLimit : 0;

  bool r = false, g = false, y = false, b = false;

  if (peopleCount == 0) {
    g = true; b = true;              // Calm - cyan
  } else if (ratio < 0.4) {
    g = true;                        // Settling - green
  } else if (ratio < 0.7) {
    y = true;                        // Moderate - yellow
  } else if (ratio < 1.0) {
    y = true; r = true;              // Energetic - orange
  } else {
    r = true;                        // Full - red
  }

  digitalWrite(LED_RED, r ? HIGH : LOW);
  digitalWrite(LED_GREEN, g ? HIGH : LOW);
  digitalWrite(LED_YELLOW, y ? HIGH : LOW);
  digitalWrite(LED_BLUE, b ? HIGH : LOW);
}

// ---------------- Activity LED (D12, direct-driven) ----------------
// Two independent ways this LED turns ON:
//   1) peopleCount > 0  -> stays solidly ON for as long as anyone is
//                          in the room, no timeout, turns OFF only
//                          when peopleCount drops back to 0.
//   2) Vibration/tap detected while the room is EMPTY -> brief ON
//      blip for ACTIVITY_HOLD_MS, then back OFF (unchanged behavior).
void triggerActivityLed() {
  digitalWrite(ACTIVITY_LED, HIGH);
  activityLedOn = true;
  activityLedOffTime = millis() + ACTIVITY_HOLD_MS;
}

void updateActivityLed() {
  // Room occupied -> force LED on, ignore any pending timeout entirely.
  if (peopleCount > 0) {
    digitalWrite(ACTIVITY_LED, HIGH);
    activityLedOn = true;
    return;
  }

  // Room empty -> fall back to the normal timed vibration blip.
  if (activityLedOn && millis() > activityLedOffTime) {
    digitalWrite(ACTIVITY_LED, LOW);
    activityLedOn = false;
  }
}

// ---------------- MPU-6050 (raw register access, clone-friendly) ----------------
void mpuInit() {
  // Wake the MPU-6050 up (it starts in sleep mode by default)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_REG_PWR_MGMT_1);
  Wire.write(0x00);
  uint8_t result = Wire.endTransmission();

  if (result == 0) {
    mpuFound = true;
    Serial.println(F("MPU-6050 responded OK (raw I2C)."));
  } else {
    mpuFound = false;
    Serial.println(F("MPU-6050 not responding! Check wiring/address (AD0->GND = 0x68)."));
  }
}

// Averages several accel readings right at boot (board must be sitting
// still/flat during this) to find this specific sensor's real resting
// magnitude, instead of assuming a textbook 1.0g. This compensates for
// the offset error common on cloned MPU-6050 boards.
void calibrateMpuBaseline() {
  Serial.println(F("Calibrating MPU-6050 baseline - keep it still..."));
  const int SAMPLES = 50;
  float sum = 0;
  int validSamples = 0;

  for (int i = 0; i < SAMPLES; i++) {
    float m;
    if (mpuReadAccelG(m)) {
      sum += m;
      validSamples++;
    }
    delay(20);
  }

  if (validSamples > 0) {
    mpuRestBaselineG = sum / validSamples;
  }
  Serial.print(F("MPU baseline set to: "));
  Serial.print(mpuRestBaselineG, 3);
  Serial.println(F("g"));
}

// Reads accel X/Y/Z and returns the magnitude in g's.
// Uses default sensitivity (+-2g range => 16384 LSB/g).
bool mpuReadAccelG(float &magG) {
  if (!mpuFound) return false;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_REG_ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0) {
    mpuFound = false; // lost connection, will need re-check
    return false;
  }

  uint8_t bytesReceived = Wire.requestFrom((int)MPU_ADDR, 6);
  if (bytesReceived < 6) return false;

  int16_t rawX = (Wire.read() << 8) | Wire.read();
  int16_t rawY = (Wire.read() << 8) | Wire.read();
  int16_t rawZ = (Wire.read() << 8) | Wire.read();

  float ax = rawX / 16384.0;
  float ay = rawY / 16384.0;
  float az = rawZ / 16384.0;

  magG = sqrt(ax * ax + ay * ay + az * az);
  return true;
}

// ---------------- OLED display ----------------
void updateDisplay(float vibeMag, bool vibrationDetected, bool mpuOk) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print(F("AURA"));

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(F("Count: "));
  display.print(peopleCount);
  display.print(F(" / "));
  display.println(capacityLimit);

  display.setCursor(0, 32);
  if (peopleCount == 0) display.println(F("Status: CALM"));
  else {
    float ratio = (float)peopleCount / capacityLimit;
    if (ratio < 0.4) display.println(F("Status: SETTLING"));
    else if (ratio < 0.7) display.println(F("Status: MODERATE"));
    else if (ratio < 1.0) display.println(F("Status: ENERGETIC"));
    else display.println(F("Status: FULL"));
  }

  display.setCursor(0, 44);
  if (!mpuOk) {
    display.println(F("Vibe: MPU offline"));
  } else {
    display.print(F("Vibe: "));
    display.print(vibeMag, 2);
    display.print(F("g (base "));
    display.print(mpuRestBaselineG, 2);
    display.print(F(")"));
    if (vibrationDetected) {
      display.print(F(" !"));
      if (millis() - lastVibrationShown > 3000) {
        Serial.println(F("Vibration detected"));
        lastVibrationShown = millis();
      }
    }
  }

  display.display();
}

// ---------------- Bluetooth status ----------------
void sendBluetoothStatus() {
  btSerial.print(F("COUNT:"));
  btSerial.print(peopleCount);
  btSerial.print(F("/"));
  btSerial.println(capacityLimit);
}
