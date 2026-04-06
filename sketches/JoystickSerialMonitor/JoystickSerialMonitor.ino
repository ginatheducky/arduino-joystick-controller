// ginatheducky
// 04-04-2026

// ------- PINS -------
# define X_PIN A0
# define Y_PIN A1
# define BTN_PIN A2

// ------- JOYSTICK SETUP -------
// measured centers
const int X_CENTER = 509;
const int Y_CENTER = 516;

// ADC range for Arduino Uno
// ADC: Uno is 10-bit (0..1023). Arduino docs note analogRead() returns a range based on ADC resolution. [4](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/)
constexpr int ADC_BITS = 10;
constexpr int ADC_MIN  = 0;
constexpr int ADC_MAX  = (1 << ADC_BITS) - 1;


// Deadzone as a percentage of half-range (e.g., 5%)
// We'll compute per-axis deadzones in raw ADC counts
const float DEADZONE_PERC = 0.05f; // 5%

// Compute per-axis deadzones based on the *smaller* of the two half-ranges.
// This keeps the feel symmetric even if the center is not exactly in the middle.
const int X_DEADZONE = (int)(min(X_CENTER - ADC_MIN, ADC_MAX - X_CENTER) * DEADZONE_PERC);
const int Y_DEADZONE = (int)(min(Y_CENTER - ADC_MIN, ADC_MAX - Y_CENTER) * DEADZONE_PERC);

// ---- Struct to return both values ----
struct JoyXY {
  float x;
  float y;
};

// HELPER FUNCTIONS
// Clamp helper
static inline float clampUnit(float v) {
  if (v > 1.0f)  return 1.0f;
  if (v < -1.0f) return -1.0f;
  return v;
}

// print values to console helper
void printToConsole(float x, float y) {
  Serial.print("x, y: ");
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);
}

/**
* Map a raw ADC value to [-1, 1] using a manual center
* below center: scale using distance to ADC_MIN
* above center: scale using distance to ADC_MAX
*/
float mapValues(int raw, int center, int deadzone, int minADC, int maxADC) {
  int delta = raw - center;
  // This delta tells you everything:
  // delta == 0 → joystick is centered
  // delta < 0 → joystick moved left/down
  // delta > 0 → joystick moved right/up

  // Deadzone
  if (abs(delta) < deadzone) return 0.0f;

  // Scale based on which side of center we are on
  if (delta < 0) {
    float v = (float)delta / (float)(center - minADC);      // negative
    return clampUnit(v);
  } else {
    float v = (float)delta / (float)(maxADC - center);      // positive
    return clampUnit(v);
  }
}


// ---- One function that returns both mapped axis values ----
JoyXY getJoystickValues(int x_raw, int y_raw) {
  JoyXY out;
  out.x = mapValues(x_raw, X_CENTER, X_DEADZONE, ADC_MIN, ADC_MAX);
  out.y = mapValues(y_raw, Y_CENTER, Y_DEADZONE, ADC_MIN, ADC_MAX);
  return out;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read the ADC values
  int x_raw = analogRead(X_PIN);
  int y_raw = analogRead(Y_PIN);

  // one line to map both values (returns a struct)
  JoyXY joy = getJoystickValues(x_raw, y_raw);

  // uncomment this line to get the center values
  //printToConsole(x_raw, y_raw);

  // comment this line out to get the raw center values 
  printToConsole(joy.x, joy.y);

}