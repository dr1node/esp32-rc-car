#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled in this ESP32 build. Check your board and Arduino core."
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error "Bluetooth Classic SPP is not available on this board. Use a compatible ESP32 board."
#endif

BluetoothSerial SerialBT;


constexpr int IN1 = 26;
constexpr int IN2 = 27;
constexpr int IN3 = 14;
constexpr int IN4 = 13;
constexpr int ENA = 25;  
constexpr int ENB = 33;  


constexpr int PWM_FREQUENCY = 1000;
constexpr int PWM_RESOLUTION = 8; 

int speedValue = 180;  
unsigned long lastCommandMs = 0;
constexpr unsigned long COMMAND_TIMEOUT_MS = 1500;

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  
  digitalWrite(IN1, leftSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN2, leftSpeed >= 0 ? LOW : HIGH);
  ledcWrite(ENA, abs(leftSpeed));

  
  digitalWrite(IN3, rightSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN4, rightSpeed >= 0 ? LOW : HIGH);
  ledcWrite(ENB, abs(rightSpeed));

  
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
    
      setMotorSpeeds(-speedValue, speedValue);
      Serial.println("Left");
      break;

    case 'R':
    case 'r':
     
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


  if (millis() - lastCommandMs > COMMAND_TIMEOUT_MS) {
    stopCar();
  }

  delay(5);
}
