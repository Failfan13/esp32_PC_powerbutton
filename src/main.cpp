#include <OneButton.h>

// Set button & relay pin
OneButton button (0, true);
const int relaySwitch = 2;

int poweredOn = false;
int buttonDown = 0;
int buttonUp = 0;

void OnceButtonClick();
void HoldButtonClick();
int IsLampOn();

void setup() {
  Serial.begin(115200);
  // Map relaySwitch variable to output
  pinMode(relaySwitch, OUTPUT);

  // OneButton attachments
  button.attachClick(OnceButtonClick);
  button.attachLongPressStop(HoldButtonClick);
}


void loop() {
  // Look for button changes
  button.tick();

  delay(10);
}

void OnceButtonClick() {
  if (IsLampOn()) {
    Serial.println("Turning light on");
    poweredOn = true;
    digitalWrite(relaySwitch, HIGH);
  }
  else {
    Serial.println("Turning light off");
    poweredOn = false;
    digitalWrite(relaySwitch, LOW);
  }
}

void HoldButtonClick() {
  if (IsLampOn()) {
    Serial.println("Flickering light on");
    for (int i=0;i<5;i++) {
      digitalWrite(relaySwitch, HIGH);
      delay(200);
      digitalWrite(relaySwitch, LOW);
      delay(200);
      digitalWrite(relaySwitch, HIGH);
      delay(200);
    }
    poweredOn = true;
  }
  else {
    OnceButtonClick();
  }
}

// Check if light is powered on
int IsLampOn() {
  if (poweredOn) {
    return 0;
  }
  return 1;
}
