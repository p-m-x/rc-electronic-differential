#include <Arduino.h>

#define PW_LEFT_MAX 600
#define PW_RIGHT_MAX 2400
#define PW_FORWARD_MAX 2400
#define PW_BACk_MAX 600
#define PW_ZERO_TOLERATION 5
#define PW_AVG_SAMPLE_SIZE 20
#define SPEED_DIFF_RATIO 0.2
#define SERVO_THROTLE_PIN 12
#define SERVO_DIRECTION_PIN 13

// https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
#define ML_RPWM 3
#define ML_LPWM 9
#define MR_RPWM 10
#define MR_LPWM 11

#define ML_EN_R A1
#define ML_EN_L A2
#define MR_EN_R A3
#define MR_EN_L A4

unsigned long servoTimeout = 3000;
unsigned long PWServoThrottle = 0;
unsigned long PWServoDirection = 0;
unsigned long directionNeutralPosition = 0;
unsigned long throttleNeutralPosition = 0;

void setupHardware();
void move(int motorLPWM, int motorRPWM, bool forward);
void stop(boolean freewheel) ;
float normalizeServoValue(unsigned long servoPW, unsigned long neutralPosition, int scope);

void setup() {
  setupHardware();
  directionNeutralPosition = PW_LEFT_MAX + (PW_RIGHT_MAX - PW_LEFT_MAX) / 2;
  throttleNeutralPosition = PW_RIGHT_MAX + (PW_FORWARD_MAX - PW_BACk_MAX) / 2;
}

void loop() {
  PWServoThrottle = pulseIn(SERVO_THROTLE_PIN, HIGH, servoTimeout);
  PWServoDirection = pulseIn(SERVO_DIRECTION_PIN, HIGH, servoTimeout);
  float direction = normalizeServoValue(PWServoDirection, directionNeutralPosition, PW_RIGHT_MAX - PW_LEFT_MAX);
  float throttle = normalizeServoValue(PWServoThrottle, throttleNeutralPosition, PW_FORWARD_MAX - PW_BACk_MAX);

}

// move forward = true, move back = false
void move(int motorLPWM, int motorRPWM, bool forward) {
  digitalWrite(ML_EN_L, !forward);
  digitalWrite(ML_EN_R, forward);
  digitalWrite(MR_EN_L, !forward);
  digitalWrite(MR_EN_R, forward);

  if (forward) {
    digitalWrite(ML_LPWM, LOW);
    analogWrite(ML_RPWM, motorLPWM);

    digitalWrite(MR_LPWM, LOW);
    analogWrite(MR_RPWM, motorRPWM);
  } else {
    digitalWrite(ML_RPWM, LOW);
    analogWrite(ML_LPWM, motorLPWM);

    digitalWrite(MR_RPWM, LOW);
    analogWrite(MR_LPWM, motorRPWM);
  }  
}

void stop(boolean freewheel) {
  digitalWrite(ML_EN_L, !freewheel);
  digitalWrite(ML_EN_R, !freewheel);
  digitalWrite(MR_EN_L, !freewheel);
  digitalWrite(MR_EN_R, !freewheel);
  digitalWrite(ML_LPWM, LOW);
  digitalWrite(ML_RPWM, LOW);
  digitalWrite(MR_LPWM, LOW);
  digitalWrite(MR_RPWM, LOW);
}

void setupHardware() {
  pinMode(SERVO_DIRECTION_PIN, INPUT);
  pinMode(SERVO_THROTLE_PIN, INPUT);
  pinMode(ML_EN_L, OUTPUT);
  pinMode(ML_EN_R, OUTPUT);
  pinMode(MR_EN_L, OUTPUT);
  pinMode(MR_EN_R, OUTPUT);
  digitalWrite(ML_EN_L, LOW);
  digitalWrite(ML_EN_R, LOW);
  digitalWrite(MR_EN_L, LOW);
  digitalWrite(MR_EN_R, LOW);
  Serial.begin(9600);
}

float normalizeServoValue(unsigned long servoPW, unsigned long neutralPosition, int scope) {

  if (servoPW < neutralPosition - PW_ZERO_TOLERATION) {
    return (servoPW - neutralPosition) / (scope / 2);
  } else if (servoPW > neutralPosition + PW_ZERO_TOLERATION) {
    return (servoPW - neutralPosition) / (scope / 2);
  }
  return 0.0;
}