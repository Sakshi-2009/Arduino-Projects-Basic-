#include <Bluepad32.h>

#define PWM1_PIN 12
#define DIR1_PIN 26 
#define PWM2_PIN 14
#define DIR2_PIN 27
#define PWM3_PIN 19
#define DIR3_PIN 18
#define PWM4_PIN 17
#define DIR4_PIN 4


ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller connected, index=%d\n", i);
            myControllers[i] = ctl;
            return;
        }
    }
    Serial.print("CALLBACK: No empty slot for new controller.");
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            return;
        }
    }
    Serial.print("CALLBACK: Controller disconnected, but not found.");
}

void setup() {
    Serial.begin(115200);
      pinMode(PWM1_PIN, OUTPUT);
  pinMode(DIR1_PIN, OUTPUT);
  ledcAttachPin(PWM1_PIN, 0); // Channel 0 for Motor 1
  ledcSetup(0, 1000, 8);      // 1 kHz, 8-bit resolution

  pinMode(PWM2_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
  ledcAttachPin(PWM2_PIN, 1); // Channel 0 for Motor 1
  ledcSetup(1, 1000, 8);      // 1 kHz, 8-bit resolution

  pinMode(PWM3_PIN, OUTPUT);
  pinMode(DIR3_PIN, OUTPUT);
  ledcAttachPin(PWM3_PIN, 2); // Channel 0 for Motor 1
  ledcSetup(2, 1000, 8);      // 1 kHz, 8-bit resolution

  pinMode(PWM4_PIN, OUTPUT);
  pinMode(DIR4_PIN, OUTPUT);
  ledcAttachPin(PWM4_PIN, 3); // Channel 0 for Motor 1
  ledcSetup(3, 1000, 8);      // 1 kHz, 8-bit resolution

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %02X:%02X:%02X:%02X:%02X:%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);
    //delay(3000);
}

void loop() {
int L1;
int R1;
int L2;
int R2;
int UP;
int DOWN;
int LEFT;
int RIGHT;
int Y;
int X;
int A;
int B;
int START;
int BACK;
int RX;
int RY;
int LX;
int LY;
    bool dataUpdated = BP32.update();
    if (!dataUpdated) {
        return; // No new data; exit the loop
    }

    for (auto myController : myControllers) {
        if (myController && myController->isConnected()) {
            // Access D-pad state
            int dpad = myController->dpad();
            UP = (dpad & 0x01) ? 1 : 0;
            DOWN=(dpad & 0x02) ? 1 : 0;
            RIGHT=(dpad & 0x04) ? 1 : 0;
            LEFT=(dpad & 0x08) ? 1 : 0;
           

            // Access button state
            int buttons = myController->buttons();
            A=(buttons & 0x0001) ? 1 : 0;
            B=(buttons & 0x0002) ? 1 : 0;
            X=(buttons & 0x0004) ? 1 : 0;
            Y=(buttons & 0x0008) ? 1 : 0;
            L1=(buttons & 0x0010) ? 1 : 0;
            R1=(buttons & 0x0020) ? 1 : 0;
            
            // if (buttons & 0x0040) {
            //     L2=1;
            // }
            // else{
            //   L2=0;
            // }
            // if (buttons & 0x0080) {
            //     R2=1;
            // }
            // else{
            //   R2=0;
            // }
            
            

            // Access misc button state
            int miscButtons = myController->miscButtons();
            START= (miscButtons & 0x02) ? 1 : 0;
            BACK= (miscButtons & 0x04) ? 1 : 0;
            

            // Access axis values
            int axisLX = myController->axisX();  // Left stick X-axis
            int axisLY = myController->axisY();  // Left stick Y-axis
            int axisRX = myController->axisRX(); // Right stick X-axis
            int axisRY = myController->axisRY(); // Right stick Y-axis

            axisLX=map(axisLX,-512,512,-127,127);
            LX=constrain(axisLX,-127,127);
            axisLY=map(axisLY,-512,512,-127,127);
            LY=constrain(axisLY,-127,127);
            axisRX=map(axisRX,-512,512,-127,127);
            RX=constrain(axisRX,-127,127);
            axisRY=map(axisRY,-512,512,-127,127);
            RY=constrain(axisRY,-127,127);
            RY=-RY;
            LY=-LY;

            if(RX>-25 && RX<25) RX=0;
            if(RY>-25 && RY<25) RY=0;
            if(LX>-25 && LX<25) LX=0;
            if(LY>-25 && LY<25) LY=0;
                  
            // // Access brake and throttle
            L2 = myController->brake();
            R2 = myController->throttle();
            
           
        }
    }


    
    Serial.print(" L2:" );
    Serial.print(L2);
    Serial.print(" R2:" );
    Serial.print(R2);
    if(L2>25){
digitalWrite(DIR1_PIN, LOW);
  ledcWrite(0, 255); // 50% duty cycle (128/255)
    }
   else if(R2>25){
    digitalWrite(DIR1_PIN, HIGH);
  ledcWrite(0, 255); // 50% duty cycle (128/255)
  }
  else
  {
  digitalWrite(DIR1_PIN, HIGH);
  ledcWrite(0, 0); // 50% duty cycle (128/255)
  }
  if(L2>25){
digitalWrite(DIR2_PIN, HIGH);
  ledcWrite(1, 255); // 50% duty cycle (128/255)
    }
   else if(R2>25){
    digitalWrite(DIR2_PIN, LOW);
  ledcWrite(1, 255); // 50% duty cycle (128/255)
  }
  else
  {
  digitalWrite(DIR2_PIN, HIGH);
  ledcWrite(1, 0); // 50% duty cycle (128/255)
  }
  if(L2>25){
digitalWrite(DIR3_PIN, LOW);
  ledcWrite(2, 255); // 50% duty cycle (128/255)
    }
   else if(R2>25){
    digitalWrite(DIR3_PIN, HIGH);
  ledcWrite(2, 255); // 50% duty cycle (128/255)
  }
  else
  {
  digitalWrite(DIR3_PIN, HIGH);
  ledcWrite(2, 0); // 50% duty cycle (128/255)
  }
  if(L2>25){
digitalWrite(DIR4_PIN, LOW);
  ledcWrite(3, 255); // 50% duty cycle (128/255)
    }
   else if(R2>25){
    digitalWrite(DIR4_PIN, HIGH);
  ledcWrite(3, 255); // 50% duty cycle (128/255)
  }
  else
  {
  digitalWrite(DIR4_PIN, HIGH);
  ledcWrite(3, 0); // 50% duty cycle (128/255)
  }

  Serial.print("LX: ");
    Serial.print(LX);
    Serial.print(" LY: ");
    Serial.print(LY);
    Serial.print(" RX: ");
    Serial.println(RX);
    Serial.println();
}