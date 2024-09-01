#include <BleCombo.h>
#include <Preferences.h>

#define redPin 4
#define greenPin 16
#define bluePin 17

#define xPin 18
#define yPin 19
#define pushPin 21

#define leftPin 26
#define spacePin 25
#define rightPin 33
#define altTabPin 32
#define downPin 35
#define upPin 34

int lastClickState = HIGH;
int lastLeftState = HIGH;
int lastSpaceState = HIGH;
int lastRightState = HIGH;
int lastAltTabState = HIGH;
int lastDownState = HIGH;
int lastUpState = HIGH;

float lightMultiplier = 1.0;

Preferences preferences;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(pushPin, INPUT_PULLUP);
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(spacePin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);
  pinMode(altTabPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(upPin, INPUT_PULLUP);

  preferences.begin("main", false);
  lightMultiplier = preferences.getString("brightness", "1.0").toFloat();

  Keyboard.deviceName = "Computer Remote";
  Keyboard.deviceManufacturer = "Richard Xiong";
  Keyboard.begin();
  Mouse.begin();
}

int rgbStatus = 0;

int numberPeak(int peak) {
  int distance = 255 - abs(rgbStatus - peak);
  if (distance < 0) return 0;
  return floor(distance * lightMultiplier);
}

int readJoystick(int pin) {
  int reading = (analogRead(pin) - 2048) / 128;
  if (abs(reading) < 2) return 0;
  return reading * abs(reading) / 20;
}

void loop() {
  int downState = digitalRead(downPin);
  int upState = digitalRead(upPin);
  int x = readJoystick(xPin);
  int y = readJoystick(yPin);
  int leftState = digitalRead(leftPin);
  int spaceState = digitalRead(spacePin);
  int rightState = digitalRead(rightPin);
  int altTabState = digitalRead(altTabPin);
  int pushState = digitalRead(pushPin);

  if (upState == LOW && downState == LOW && y != 0) {
    if (y < 0) {
      lightMultiplier = min(lightMultiplier + 0.01, 1.0);
    } else {
      lightMultiplier = max(lightMultiplier - 0.01, 0.0);
    }
    preferences.putString("brightness", String(lightMultiplier));
  }

  analogWrite(redPin, numberPeak(255));
  analogWrite(greenPin, numberPeak(255 * 2));
  analogWrite(bluePin, max(numberPeak(0), numberPeak(255 * 3)));
  rgbStatus = (rgbStatus + 1) % (255 * 3);

  // analogWrite(redPin, floor(255 * lightMultiplier));

  if (!Keyboard.isConnected()) return delay(10);

  if (x != 0 || y != 0) {
    Mouse.move(x, y);
  }

  if (pushState != lastClickState) {
    if (pushState == LOW) {
      Mouse.press(MOUSE_LEFT);
    } else {
      Mouse.release(MOUSE_LEFT);
    }
    lastClickState = pushState;
  }

  if (leftState != lastLeftState) {
    if (leftState == LOW) {
      Keyboard.press(KEY_LEFT_ARROW);
    } else {
      Keyboard.release(KEY_LEFT_ARROW);
    }
    lastLeftState = leftState;
  }

  if (spaceState != lastSpaceState) {
    if (spaceState == LOW) {
      Keyboard.press(' ');
    } else {
      Keyboard.release(' ');
    }
    lastSpaceState = spaceState;
  }

  if (rightState != lastRightState) {
    if (rightState == LOW) {
      Keyboard.press(KEY_RIGHT_ARROW);
    } else {
      Keyboard.release(KEY_RIGHT_ARROW);
    }
    lastRightState = rightState;
  }

  if (altTabState != lastAltTabState) {
    if (altTabState == LOW) {
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_TAB);
      Keyboard.release(KEY_TAB);
      Keyboard.release(KEY_LEFT_ALT);
    }
    lastAltTabState = altTabState;
  }

  if (downState != lastDownState) {
    if (downState == LOW) {
      Keyboard.press(KEY_DOWN_ARROW);
    } else {
      Keyboard.release(KEY_DOWN_ARROW);
    }
    lastDownState = downState;
  }

  if (upState != lastUpState) {
    if (upState == LOW) {
      Keyboard.press(KEY_UP_ARROW);
    } else {
      Keyboard.release(KEY_UP_ARROW);
    }
    lastUpState = upState;
  }

  delay(10);
}