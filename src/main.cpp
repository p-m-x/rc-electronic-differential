#include <Arduino.h>
#include <BTS7960.h>

#define PW_LEFT_MAX 600
#define PW_RIGHT_MAX 2400
#define PW_FORWARD_MAX 2400
#define PW_BACKWARD_MAX 600
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

#define DEBUG 0

#define MOVE_DIR_FORWARDS 0
#define MOVE_DIR_BACKWARDS 1

unsigned long servoTimeout = 3000;
unsigned long PWServoThrottle = 0;
unsigned long PWServoDirection = 0;
unsigned long directionNeutralPosition = 0;
unsigned long throttleNeutralPosition = 0;

BTS7960 motorLeft(ML_EN_R, ML_RPWM,  ML_EN_L, ML_LPWM, DEBUG);
BTS7960 motorRight(MR_EN_R, MR_RPWM, MR_EN_L, MR_LPWM, DEBUG);

void setupHardware();
void move(int motorLeftSpeed, int motorRightSpeed, int dir);
void stop(boolean freewheel) ;
int normalizeServoValue(unsigned long servoPW, unsigned long neutralPosition, int scope);
void calculateMotorsSpeed(int &motorLeftSpeed, int &motorRightSpeed, int direction);

void setup() {
  setupHardware();
  directionNeutralPosition = PW_LEFT_MAX + (PW_RIGHT_MAX - PW_LEFT_MAX) / 2;
  throttleNeutralPosition = PW_BACKWARD_MAX + (PW_FORWARD_MAX - PW_BACKWARD_MAX) / 2;

  if (DEBUG) {
    Serial.print(F("Direction servo neutral RAW value: "));
    Serial.println(directionNeutralPosition);
    Serial.print(F("Throttle servo neutral RAW value: "));
    Serial.println(throttleNeutralPosition);
  }
}

void loop() {
  PWServoThrottle = pulseIn(SERVO_THROTLE_PIN, HIGH, servoTimeout);
  PWServoDirection = pulseIn(SERVO_DIRECTION_PIN, HIGH, servoTimeout);

  int direction = normalizeServoValue(PWServoDirection, directionNeutralPosition, PW_RIGHT_MAX - PW_LEFT_MAX);
  int throttle = normalizeServoValue(PWServoThrottle, throttleNeutralPosition, PW_FORWARD_MAX - PW_BACKWARD_MAX);
  int motorLeftSpeed = abs(throttle);
  int motorRightSpeed = abs(throttle);
  calculateMotorsSpeed(motorLeftSpeed, motorRightSpeed, direction);

  if (DEBUG) {
    Serial.print("THROTTLE: ");
    Serial.print(throttle);
    Serial.print("  DIR: ");
    Serial.print(direction);
    Serial.print("  SPEED L: ");
    Serial.print(motorLeftSpeed);
    Serial.print("  SPEED R: ");
    Serial.println(motorRightSpeed);
  }
  
  if (throttle > 0) {    
    move(motorLeftSpeed, motorRightSpeed, MOVE_DIR_FORWARDS);
  } else if (throttle < 0) {
    move(motorLeftSpeed, motorRightSpeed, MOVE_DIR_BACKWARDS);
  } else {
    stop(false);
  }
}

void move(int motorLeftSpeed, int motorRightSpeed, int dir) {
  motorLeft.rotate(motorLeftSpeed, dir);
  motorRight.rotate(motorRightSpeed, dir);
}

void stop(boolean freewheel) {
  motorLeft.stop();
  motorRight.stop();
}

void setupHardware() {
  pinMode(SERVO_DIRECTION_PIN, INPUT);
  pinMode(SERVO_THROTLE_PIN, INPUT);
  Serial.begin(9600);
}

int normalizeServoValue(unsigned long servoPW, unsigned long neutralPosition, int scope) {

  if ((servoPW < neutralPosition - PW_ZERO_TOLERATION) || (servoPW > neutralPosition + PW_ZERO_TOLERATION)) {
    long p = servoPW - neutralPosition;
    return (float(p) / float(scope / 2)) * 100;
  }
  return 0;
}

void calculateMotorsSpeed(int &motorLeftSpeed, int &motorRightSpeed, int direction) {

  // turn right
  if (direction > 0) {
    motorRightSpeed -= motorRightSpeed * direction/100 * SPEED_DIFF_RATIO;
  } 
  // turn left
  else if (direction < 0) {
    motorLeftSpeed -= motorLeftSpeed * direction/100 * SPEED_DIFF_RATIO;
  }

}