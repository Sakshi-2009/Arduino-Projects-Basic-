#include <Wire.h>
#include <MPU6050.h>
#include "CytronMotorDriver.h"

MPU6050 mpu;

#include <Servo.h>  // Include the Servo library

Servo myServo;  // Create a Servo object to control the motor

int angle = 0;  // Initial angle of the servo
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CytronMD motor1(PWM_DIR, 9, 8);
CytronMD motor2(PWM_DIR, 10,7);
CytronMD motor3(PWM_DIR, 5, 2);
CytronMD motor4(PWM_DIR, 6, 4);
// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values

float yaw = 0;


float kp = 0.0;
float ki = 0.0;
float kd = 0.0;

float setP = 0.0;
float error = 0.0;
float Perror = 0.0;
float proportional = 0;
float integral = 0;
float derivative = 0;
float output = 0;

float resultant = 0;
float theta = 0;

float V1 = 0;
float V2 = 0;
float V3 = 0;

int i = 0, j = 0, k = 0, l = 0, m, n, p;
float Vy;
float Vx;
float a = 0;
float b = 0;
float c = 0;




void setup() {
  Serial.begin(115200);

  myServo.attach(3);


  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }


  mpu.calibrateGyro();

  mpu.setThreshold(1);
}

void loop() {
  timer = millis();


  Vector norm = mpu.readNormalizeGyro();


  yaw = yaw + norm.ZAxis * timeStep;


  if (i <= 1000) {
    
    Vx = 0;
    Vy = 80;
    i++;
    Serial.print("forward");
  } else if (k <= 800 && i >= 1000) {
    motor4.setSpeed(-20);
    Serial.print("ig32 down ");
    k++;

  } else if (l < 1 && k >= 800) {
    Serial.print("servo on  ");
    myServo.write(180);
    motor4.setSpeed(0);
    delay(2000);
    
    l++;
  } else if (m <= 800 && l >= 1) {
    motor4.setSpeed(20);
    Serial.print("ig32 up ");
    m++;
  } else if (j <= 1000 && m > 800) {
    j++;
    Vx = 0;
    Vy = -80;
    motor4.setSpeed(0);
    Serial.print("Backward");
  } else if (n <= 800 && j > 1000) {
    motor4.setSpeed(-20);
    Serial.print("ig32 down   ");
    n++;

  } else if (p <= 1 && n > 800) {
    Serial.print("servo off  ");
    myServo.write(0);
    motor4.setSpeed(-0);
    delay(2000);
    p++;
  } else {

    Vx = 0;
    Vy = 0;
    Serial.print("Stop");
  }

  resultant = sqrt((Vx * Vx) + (Vy * Vy));
  theta = atan2(Vy, Vx);
  theta = theta * (180 / 3.14);



  error = setP - yaw;
  proportional = error;
  integral = integral + error;
  derivative = error - Perror;
  Perror = error;

  kp = 6.0;  //5
  ki = 0.0;
  kd = 30.0;  //125

  output = kp * proportional + ki * integral + kd * derivative;

  V1 = resultant * cos(theta * (3.14 / 180));
  V2 = resultant * sin((theta - 30) * (3.14 / 180));
  V3 = resultant * sin((theta + 30) * (3.14 / 180));

  a = V1 + output;
  b = V2 + output;
  c = V3 - output;

  a = constrain(a, -40, 40);
  c = constrain(c, -40, 40);
  b = constrain(b, -40, 40);

  motor3.setSpeed(a);
  motor2.setSpeed(b);  // b
  motor1.setSpeed(c);  // c




  // Output raw

  Serial.print(" Yaw = ");
  Serial.print(yaw);

  Serial.print(" a = ");
  Serial.print(a);
  Serial.print(" b = ");
  Serial.print(b);
  Serial.print(" c = ");
  Serial.println(c);
  
}