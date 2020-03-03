// Authors: Tyler B. and Joe L. 
// Course:  EENG350 -- SEED Lab
// Date:    2020-02-26

// Purpose: To fulfill the MiniProject guidelines. This program controls an
//    Arduino that communicates with a Raspberry Pi. Over I2C it gets a location
//    command and it attempts to turn a wheel to the specified location. It will 
//    fight against any attempt to disturb such movement. 

// Setup:   Attach the motor driver shield to the Arduino. Attach pins 2 and 6 to 
//    the encoder pins on the motor. Attach SDA and SCL lines to their respective 
//    lines on the RasPi. Ground all modules together. 

#include <Wire.h>
#include <Encoder.h>

//i2c handling
#define ADDRESS 0x45 //device address
#define RESET 0 //reset position command

//setpoint commands
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

//pin assignments & constants
double CYCLE = 255; //max PWM duty
int ENABLE = 4; //enable pin on shield 
int M1DIR = 7, M2DIR = 8, MOTOR1 = 9, MOTOR2 = 10; // motor assignment pins
int SWITCH = 5; //switch pin
int STATUS = 12; //status pin
double CPR = 3200; //counts per rotation
int M1P2 = 6, M1P1 = 2; //Motor 1 encoder pins
int M2P1 = 3, M2P2 = 11; //Motor 2 encoder pins c v
int period = 8; //velocity survey period

//location & command trackers
double currPosition1 = 0, lastPosition1= 0, lastTime = 0 , lastPosition2 = 0, currPosition2 = 0;
double currVel1 = 0;
long count = 0;
double currTime = 0;
double VCC = 5;
double pi = 3.14159, currVel2 = 0;
double rotVel = 0, forwardsVel = 0;
double setRotVel = 0, setForwardsVel = 0;
double M1Volt = 0, M2Volt = 0; //Calculated voltage  to send to the motors (M1 = right, M2 = left);

//PID control
double PControlForwards =  0.326699545006117;
double IControlForwards =  1.66682107862121;
double PControlRot = 0.907700082980526;
double IControlRot = 4.10734287548688;
double toWrite = 0;
long count2 = 0;
double VaBar = 0, VaDelta = 0; //Voltage commands to the motors to calc based on desired velocities
double d = (7 + 7/8) * 0.0254; //meters between wheels
double r = ((5 + 7/8) / 2) * 0.0254; //meters radius

double intgForwards = 0;
double intgRot = 0;

Encoder M1(M1P1, M1P2);
Encoder M2(M2P1, M2P2);

void setup() {

  //Connect a switch between pin 4 and ground
  //initialize motor pins
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(ENABLE, OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(STATUS, INPUT);

  //ensure encoder is in pullup mode
  pinMode(M1P1, INPUT_PULLUP);
  pinMode(M1P2, INPUT_PULLUP);
  pinMode(M2P1, INPUT_PULLUP);
  pinMode(M2P2, INPUT_PULLUP);

  //initialize serial comms
  Serial.begin(115200);

  //init. time tracker
    lastTime = millis();

  digitalWrite(ENABLE, HIGH); //Setting Low disables shield motor outputs
  digitalWrite(M1DIR, HIGH); //HI is CW, LOW is CCW
  digitalWrite(M2DIR, HIGH);
  analogWrite(MOTOR1, 0 * CYCLE); //1 * CYCLE is Max Speed, 0 * cycle is not moving
  analogWrite(MOTOR2, 0);

}

void loop() {
  currTime = millis();
  count = M1.read(); //get location of motor 1
  count2 = M2.read(); //location motor 2

    setForwardsVel = 1;  
    setRotVel = 0; 
 

  currPosition1 = count / CPR * 2 * 3.14159; //Current Position in radians
  currPosition2 = count2 / CPR * 2 * 3.14159; //Current Position in radians
  currVel1 = (currPosition1 - lastPosition1) / ((currTime - lastTime) / 1000) ; //Velocity in rad/sec
  currVel2 = (currPosition2 - lastPosition2) / ((currTime - lastTime) / 1000) ;
  
  lastPosition1 = currPosition1;
  lastPosition2 = currPosition2;

  rotVel = (currVel1 - currVel2) * r / d; //Rotational velocity of the vehicle
  forwardsVel = r * (currVel1 + currVel2) / 2; //Instanteous forwards velocity of the vehicle
  
  intgRot += (currTime - lastTime) * (setRotVel - rotVel) / 1000;
  intgForwards += (currTime - lastTime) * (setForwardsVel - forwardsVel) / 1000;
  //if (isnan(integralRot)){
  //  integralRot = 0;
  //}
  //if (isnan(integralForwards)){
  //  integralForwards = 0;
  //}

  VaBar = (PControlForwards + intgForwards * IControlForwards) * (setForwardsVel - forwardsVel);
  VaDelta = (PControlRot + intgRot * IControlRot) * (setRotVel - rotVel);

  M1Volt = (VaBar + VaDelta) / 2;
  M2Volt = (VaBar - VaDelta) / 2;

  if (M1Volt < 0){
    M1Volt *= -1;
    digitalWrite(M1DIR, LOW);
  } else {
    digitalWrite(M1DIR, HIGH);
  }

  if (M2Volt < 0){
    M2Volt *= -1;
    digitalWrite(M2DIR, LOW);
  } else {
    digitalWrite(M2DIR, HIGH);
  }

  


  //  Print out the current status:
    //Serial.print("Destination (radians): ");
    //Serial.print(destRads);
    //Serial.print("\t");
    //if (currTime < 5000 && currTime > 2000){
    Serial.print((currTime - lastTime)/1000);
    Serial.print("\t");
    Serial.print((setForwardsVel - forwardsVel));
    Serial.print("\t");
    Serial.print(((currTime - lastTime) / 1000.0) * (setRotVel - rotVel));
    Serial.print("\t");
    Serial.print(intgForwards);
    Serial.print("\t");
    Serial.print(forwardsVel);
    Serial.print("\t");
    Serial.print(currVel2);
    Serial.print("\t");
    Serial.print(currTime);
    //Serial.print("\t");
  //Serial.print(currTime);
    Serial.print("\n");
  //  }
  //  Serial.print("\t");
  //  Serial.print(currVel);
  //  Serial.print("\n");
  //  Serial.print(currTime);
  //  Serial.print("\n");
  //Serial.print("\t Motor Command (0-255): ");
  //Serial.print(toWrite);
  //Serial.print("\n");

  //write to motor
  analogWrite(MOTOR1, M1Volt / VCC * CYCLE);
  analogWrite(MOTOR2, M2Volt / VCC * CYCLE);
  while (millis() < currTime + period);  //Sample every 5ms

  lastTime = currTime;
  
}
