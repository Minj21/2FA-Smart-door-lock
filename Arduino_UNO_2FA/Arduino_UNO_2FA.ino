#include <Keypad.h>

// Strict 2FA Smart Door Lock - Arduino UNO
// Requires passcode first, then a fresh HIGH pulse on A0 from ESP32-CAM.
// Press D to manually test relay unlock from this same sketch.

const int faceSignalPin = A0;  // ESP32-CAM face signal -> Arduino A0
const int relayPin = 8;        // Relay IN -> Arduino D8
const int buzzerPin = 12;      // Buzzer signal -> Arduino D12

const int redLEDPin = A4;
const int greenLEDPin = A3;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {3, 4, 5, 6};
byte colPins[COLS] = {7, 9, 10, 11};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String enteredCode = "";
const String correctCode = "1234";

bool waitingForFace = false;
bool armedForFreshFacePulse = false;
unsigned long faceWaitStart = 0;

const unsigned long faceWaitTimeout = 30000;
const unsigned long unlockTime = 5000;

void buzzerOff() {
  digitalWrite(buzzerPin, HIGH);
}

void beepShort() {
  digitalWrite(buzzerPin, LOW);
  delay(70);
  buzzerOff();
}

void beepLong() {
  digitalWrite(buzzerPin, LOW);
  delay(400);
  buzzerOff();
}

void beepSuccess() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(buzzerPin, LOW);
    delay(100);
    buzzerOff();
    delay(100);
  }
}

bool facePulseIsStableHigh() {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(faceSignalPin) == LOW) {
      return false;
    }
    delay(10);
  }

  return true;
}

void forceRelayUnlock(const char* source) {
  Serial.print("UNLOCK START: ");
  Serial.println(source);

  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, HIGH);

  digitalWrite(relayPin, HIGH);
  Serial.println("D8 RELAY HIGH");
  delay(unlockTime);

  digitalWrite(relayPin, LOW);
  digitalWrite(greenLEDPin, LOW);
  Serial.println("D8 RELAY LOW - LOCKED AGAIN");
}

void unlockDoor() {
  Serial.println("ACCESS GRANTED BY ARDUINO");
  beepSuccess();
  forceRelayUnlock("PASSCODE + FRESH A0 FACE PULSE");
}

void denyAccess(const char* reason) {
  Serial.print("ACCESS DENIED: ");
  Serial.println(reason);

  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, HIGH);
  beepLong();
  delay(1200);
  digitalWrite(redLEDPin, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(faceSignalPin, INPUT);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  pinMode(buzzerPin, OUTPUT);
  buzzerOff();

  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, LOW);

  Serial.println("2FA SYSTEM READY - STRICT A0 VERSION");
  Serial.println("Use 1234# then show face.");
  Serial.println("Press D to test relay unlock.");
  Serial.println("A0 must be LOW after passcode, then go HIGH from ESP32-CAM.");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    beepShort();
    Serial.print("Key pressed: ");
    Serial.println(key);

    if (key == 'D') {
      forceRelayUnlock("KEY D TEST");
      enteredCode = "";
      waitingForFace = false;
      armedForFreshFacePulse = false;
      return;
    }

    if (!waitingForFace) {
      if (key >= '0' && key <= '9') {
        enteredCode += key;
        Serial.print("Entered length: ");
        Serial.println(enteredCode.length());
      }
      else if (key == '*') {
        enteredCode = "";
        Serial.println("Input cleared");
      }
      else if (key == '#') {
        if (enteredCode == correctCode) {
          Serial.println("PASSCODE CORRECT");

          if (digitalRead(faceSignalPin) == HIGH) {
            Serial.println("A0 is already HIGH. Waiting for it to go LOW before accepting face.");
            armedForFreshFacePulse = false;
          }
          else {
            Serial.println("A0 is LOW. Armed for fresh face pulse.");
            armedForFreshFacePulse = true;
          }

          Serial.println("WAITING FOR FRESH FACE SIGNAL ON A0...");
          waitingForFace = true;
          faceWaitStart = millis();
        }
        else {
          denyAccess("WRONG PASSCODE");
        }

        enteredCode = "";
      }
    }
  }

  if (waitingForFace) {
    int faceSignal = digitalRead(faceSignalPin);

    if (!armedForFreshFacePulse) {
      if (faceSignal == LOW) {
        Serial.println("A0 returned LOW. Now armed for fresh face pulse.");
        armedForFreshFacePulse = true;
      }
    }
    else if (faceSignal == HIGH && facePulseIsStableHigh()) {
      Serial.println("FRESH FACE SIGNAL RECEIVED ON A0");
      waitingForFace = false;
      armedForFreshFacePulse = false;
      unlockDoor();

      while (digitalRead(faceSignalPin) == HIGH) {
        delay(20);
      }
    }

    if (waitingForFace && millis() - faceWaitStart > faceWaitTimeout) {
      waitingForFace = false;
      armedForFreshFacePulse = false;
      denyAccess("FACE TIMEOUT");
    }
  }
}
