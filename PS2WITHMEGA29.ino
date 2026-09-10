

#include <Wire.h>
#include <MPU6050.h>
#include <PS2X_lib.h>

//MPU6050 mpu;
PS2X ps2x;  

#include <SoftwareSerial.h>
#include <SabertoothSimplified.h>

SoftwareSerial SWSerial1(NOT_A_PIN, 8);    // Motor 1 and 2
SabertoothSimplified ST1(SWSerial1);
SoftwareSerial SWSerial2(NOT_A_PIN, 13);   // Motor 3 and 4
SabertoothSimplified ST2(SWSerial2);
SoftwareSerial SWSerial3(NOT_A_PIN, 7);    // Arm motor
SabertoothSimplified ST3(SWSerial3);

SoftwareSerial mySerial(11, 10);           // Serial communication for debugging

// PS2 Controller Variables
int L1, R1, L2, R2, LY, LX, RY, RX;
int UP, DOWN, LEFT, RIGHT;
int TRIANGLE, CIRCLE, CROSS, SQUARE;
int START, SELECT, L3, R3;

// Motor and PID Control Variables
float R, theta, M, count1 = 0, count2 = 0, count3 = 0, count4 = 0;
float setpoint = 0, pre_error = 0, proportional, integral = 0, derivative, error, pid ,a , b , c ,d ,A ,B ,C ,D;
float kp = 28, ki = 0, kd = 54;

float motor_A = 0, motor_B = 0, motor_C = 0, motor_D = 0;
int speed_arm = 0;
int R1_ref = 0, L1_ref = 0;
int c_ref = 0;
int t_ref = 0;
int s_ref = 0;
int c_ref1 = 0;
int t_ref1= 0;
int s_ref1= 0;

bool solenoid_state = LOW;
bool solenoid_state_dribble = LOW;
bool solenoid_state_angle = LOW;

int solenoid_dribbling = 28;   // Pin for dribbling solenoid
int solenoid_angle = 26;       // Pin for angle solenoid
int solenoid_base = 24;        // Pin for base solenoid

unsigned long timer = 0;
float timeStep = 0.01;
float yaw = 0;

// Deadzone threshold
const int DEADZONE = 2;
int Dpad_ref1 = 0;
int Dpad_ref2 = 0;
int Dpad_ref3 = 0;
int Dpad_ref4 = 0;


// // Function to apply deadzone filtering
// int applyDeadzone(int value, int deadzone) {
//   if (abs(value - 128) < deadzone) {
//     return 0;  // Centered
//   }
//   return value - 128;
// }

void setup() {
  mySerial.begin(115200);
  SWSerial1.begin(9600);
  SWSerial2.begin(9600);
  SWSerial3.begin(9600);
  Serial.begin(115200);

  // PS2 Initialization
  int error = ps2x.config_gamepad(22, 23, 24, 25, true, true);  // PS2 pins: clock, command, attention, data
  if (error == 0) {
    Serial.println("✅ PS2 Controller Connected!");
  } else {
    Serial.println("❌ Failed to connect PS2 Controller.");
    while (1);
  }

  // // // MPU6050 Initialization
  // while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
  //   Serial.println("Could not find MPU6050, check wiring!");
  // //   delay(500);
  // // }
  // mpu.calibrateGyro();
  // mpu.setThreshold(1);

  Serial.println("Arduino Mega Ready. Waiting for PS2 input...");

  pinMode(solenoid_dribbling, OUTPUT);
  pinMode(solenoid_base, OUTPUT);
  pinMode(solenoid_angle, OUTPUT);
}

void loop() {
  // // MPU6050 gyro reading
  timer = millis();
  Vector norm = mpu.readNormalizeGyro();
  yaw = yaw + norm.ZAxis * timeStep;
  delay((timeStep * 1000) - (millis() - timer));

  // Read PS2 inputs
  ps2x.read_gamepad();

  // Button States
  L1 = ps2x.Button(PSB_L1);
  R1 = ps2x.Button(PSB_R1);
  L2 = ps2x.Button(PSB_L2);
  R2 = ps2x.Button(PSB_R2);

  L3 = ps2x.Button(PSB_L3);
  R3 = ps2x.Button(PSB_R3);
  START = ps2x.Button(PSB_START);
  SELECT = ps2x.Button(PSB_SELECT);

  TRIANGLE = ps2x.Button(PSB_TRIANGLE);
  CIRCLE = ps2x.Button(PSB_CIRCLE);
  CROSS = ps2x.Button(PSB_CROSS);
  SQUARE = ps2x.Button(PSB_SQUARE);

  // D-Pad directions
  UP = ps2x.Button(PSB_PAD_UP);
  DOWN = ps2x.Button(PSB_PAD_DOWN);
  LEFT = ps2x.Button(PSB_PAD_LEFT);
  RIGHT = ps2x.Button(PSB_PAD_RIGHT);
  LX = ps2x.Analog(PSS_LX);

  // // Analog stick values
  // LY = applyDeadzone(ps2x.Analog(PSS_LY), DEADZONE);
  // LX = applyDeadzone(ps2x.Analog(PSS_LX), DEADZONE);
  // RY = applyDeadzone(ps2x.Analog(PSS_RY), DEADZONE);
  // RX = applyDeadzone(ps2x.Analog(PSS_RX), DEADZONE);

  // Arm Motor Control
  if (RY > 2) {
   speed_arm = 40;
  }
 if (RY < -2) {
    speed_arm = -40;
  }
   else   {
    speed_arm = 0;
  }

  // Continuous Rotation Control
  if (R2>0) {
    setpoint -= 0.2;
  }
  if (L2>0) {
    setpoint += 0.2;
  }

  // 90° Turns
  if (R1==1 && R1_ref==0) {
   R1_ref=1;
  }

  if (R1== 0 && R1_ref == 1) {
    setpoint = setpoint + 50;
    R1_ref = 0;
  }

    if (L1==1 && L1_ref==0) {
   L1_ref=1;
  }

  if (L1== 0 && L1_ref == 1) {
    setpoint = setpoint + 50;
    L1_ref = 0;
  }
  


  // Base Solenoid Control
  if (CIRCLE == 1 && c_ref1 == 0) {
    solenoid_state = !solenoid_state;
    digitalWrite(solenoid_base, solenoid_state);
    c_ref1 = 1;
  }
  if (  CIRCLE == 0 && c_ref1 == 1) {
    Serial.print(" %%% ");
    c_ref1 = 0;
  }
 
   // Base Solenoid Control
  if (TRIANGLE == 1 && t_ref1 == 0) {
    solenoid_state_dribble = !solenoid_state_dribble;
    digitalWrite(solenoid_dribbling, solenoid_state_dribble);
    t_ref1 = 1;
  }
  if (TRIANGLE == 0 && t_ref1 == 1) {
    Serial.print(" ##### ");
    t_ref1 = 0;

  }

  if (SQUARE == 1 && s_ref1 == 0) {
    solenoid_state_angle = !solenoid_state_angle;
    digitalWrite(solenoid_angle, solenoid_state_angle);
    s_ref1 = 1;
  }
  if (SQUARE == 0 && s_ref1 == 1) {
    Serial.print(" $$$$$ ");
    s_ref1 = 0;
  }

  Serial.print("solenoid_state: ");
  Serial.print(solenoid_state);
  Serial.print(" solenoid_state_dribble: ");
  Serial.print(solenoid_state_dribble);
  Serial.print(" solenoid_state_angle: ");
  Serial.print(solenoid_state_angle);




  // Joystick Movement Control
  R = sqrt((LX * LX) + (LY * LY));
  theta = atan2(LY, LX) * 180 / 3.142;

   a = R * sin((theta - 45) * 3.142 / 180);
   c = R * sin((135 - theta) * 3.142 / 180);
   b = R * sin((135 - theta) * 3.142 / 180);
   d = R * sin((theta - 45) * 3.142 / 180);

  // PID Control Adjustments
  error = setpoint - yaw;
  proportional = error;
  integral += error;
  derivative = error - pre_error;
  pre_error = error;
  pid = kp * proportional + ki * integral + kd * derivative;
  // Apply PID corrections

  A= a - pid ;
  B= b - pid ;
  C= c - pid ;
  D= d - pid ;

  motor_A = constrain(A, -127, 127);
  motor_B = constrain(B , -127, 127);
  motor_C = constrain(C, -127, 127);
  motor_D = constrain(D, -127, 127);

  // Motor Control
  ST1.motor(1, motor_A);
  ST1.motor(2, motor_B);
  ST2.motor(2, motor_C);
  ST2.motor(1, motor_D);
  ST3.motor(1, speed_arm);

  // Print debug info
  Serial.print("Yaw: "); Serial.print(yaw);
  Serial.print("LX: "); Serial.print(LX);
  Serial.print(" Error: "); Serial.println(error);

}
