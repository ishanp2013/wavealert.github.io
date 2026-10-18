const int sensorPin = 2;
const int buzzerPin = 8;
const int redLed = 9;
const int greenLed = 10;
const int DETECTED = HIGH;   // use LOW if your test showed 0 when tapping

int lastState = -1;          // new

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, HIGH);          // green on = system working
  Serial.begin(9600);                    // new
}

void loop() {
  int state = digitalRead(sensorPin);    // new

  if (state != lastState) {              // new: print only when it changes
    Serial.print("Sensor output: ");     // new
    Serial.println(state);               // new
    lastState = state;                   // new
  }

  if (state == DETECTED) {               // changed: uses the variable
    digitalWrite(greenLed, LOW);         // green off while alarming

    for (int i = 0; i < 5; i++) {        // red + buzzer, 5 times
      digitalWrite(redLed, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(redLed, LOW);
      digitalWrite(buzzerPin, LOW);
      delay(150);
    }

    delay(1000);                         // let the buzzer's shaking settle
    digitalWrite(greenLed, HIGH);        // back to normal
  }
}