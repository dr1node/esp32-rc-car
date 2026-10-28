/*
  ESP32 RC Car — reconstructed example firmware

  Assumptions:
  - ESP32 board supports Bluetooth Classic SPP (BluetoothSerial).
  - One L298N dual H-bridge controls two motor groups:
      Channel A = left-side motors
      Channel B = right-side motors
  - Four DC motors are grouped two per side, subject to driver current limits.
  - Phone app sends single-character commands over Bluetooth Classic serial.

  IMPORTANT:
  This code is not the original lost source and has not been tested on the
  original physical car. Confirm your board, wiring, pin assignments, motor
  current, and driver ratings before applying power.
*/

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled in this ESP32 build. Check your board and Arduino core."
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error "Bluetooth Classic SPP is not available on this board. Use a compatible ESP32 board."
#endif

BluetoothSerial SerialBT;

// Example GPIO assignments only. Match these to your actual wiring.
constexpr int IN1 = 26;
constexpr int IN2 = 27;
constexpr int IN3 = 14;
constexpr int IN4 = 13;
constexpr int ENA = 25;  // L298N Channel A enable / PWM
constexpr int ENB = 33;  // L298N Channel B enable / PWM

// Arduino-ESP32 core 3.x uses ledcAttach/ledcWrite(pin, duty).
// For older 2.x cores, use ledcSetup/ledcAttachPin and ledcWrite(channel, duty).
constexpr int PWM_FREQUENCY = 1000;
constexpr int PWM_RESOLUTION = 8;  // duty range 0..255

int speedValue = 180;  // initial speed: 0..255
unsigned long lastCommandMs = 0;
constexpr unsigned long COMMAND_TIMEOUT_MS = 1500;

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // Left side: Channel A
  digitalWrite(IN1, leftSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN2, leftSpeed >= 0 ? LOW : HIGH);
  ledcWrite(ENA, abs(leftSpeed));

  // Right side: Channel B
  digitalWrite(IN3, rightSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN4, rightSpeed >= 0 ? LOW : HIGH);
  ledcWrite(ENB, abs(rightSpeed));

  // Explicitly stop direction outputs when speed is zero.
  if (leftSpeed == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  if (rightSpeed == 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
}

void stopCar() {
  setMotorSpeeds(0, 0);
}

void handleCommand(char command) {
  if (command >= '0' && command <= '9') {
    speedValue = map(command - '0', 0, 9, 0, 255);
    Serial.printf("Speed set to %d\n", speedValue);
    return;
  }

  switch (command) {
    case 'F':
    case 'f':
      setMotorSpeeds(speedValue, speedValue);
      Serial.println("Forward");
      break;

    case 'B':
    case 'b':
      setMotorSpeeds(-speedValue, -speedValue);
      Serial.println("Backward");
      break;

    case 'L':
    case 'l':
      // Pivot-style left turn. Adjust if the real car uses another steering setup.
      setMotorSpeeds(-speedValue, speedValue);
      Serial.println("Left");
      break;

    case 'R':
    case 'r':
      // Pivot-style right turn. Adjust if the real car uses another steering setup.
      setMotorSpeeds(speedValue, -speedValue);
      Serial.println("Right");
      break;

    case 'S':
    case 's':
      stopCar();
      Serial.println("Stop");
      break;

    case '\r':
    case '\n':
    case ' ':
      // Ignore line endings and spaces from terminal apps.
      break;

    default:
      Serial.printf("Unknown command: %c\n", command);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // ESP32 Arduino core 3.x API.
  const bool enaAttached = ledcAttach(ENA, PWM_FREQUENCY, PWM_RESOLUTION);
  const bool enbAttached = ledcAttach(ENB, PWM_FREQUENCY, PWM_RESOLUTION);

  if (!enaAttached || !enbAttached) {
    Serial.println("ERROR: PWM setup failed. Check ESP32 Arduino core version and pins.");
    while (true) {
      delay(1000);
    }
  }

  stopCar();

  SerialBT.begin("ESP32_RC_Car");
  Serial.println("ESP32 RC Car ready.");
  Serial.println("Bluetooth device name: ESP32_RC_Car");
  Serial.println("Commands: F=forward, B=back, L=left, R=right, S=stop, 0-9=speed");

  lastCommandMs = millis();
}

void loop() {
  while (SerialBT.available()) {
    const char command = static_cast<char>(SerialBT.read());
    lastCommandMs = millis();
    handleCommand(command);
  }

  // Safety behavior: stop if the Bluetooth command stream goes quiet.
  if (millis() - lastCommandMs > COMMAND_TIMEOUT_MS) {
    stopCar();
  }

  delay(5);
}
