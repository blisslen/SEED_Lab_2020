
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
int period = 5; //velocity survey period

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
double PControlForwards =  4.30043261818074;
double IControlForwards =  20.3797501775585;
double PControlRot = 0.409577456683718;
double IControlRot = 1.68761627215546;
double outerRotD = 1.31941139408475;
double outerRotP = 2.00368076718331;
double outerDistD = 1.00050168515022;
double outerDistP = 2.44853446940978;
double currDistance = 0;
double lastDistance = 0;
double setDistance = 0;
double toWrite = 0;
double count2 = 0, currAngle = 0, setAngle = 0;
double VaBar = 0, VaDelta = 0; //Voltage commands to the motors to calc based on desired velocities
double d = 0.2000025; //meters between wheels
double r = .148 / 2; //meters radius
double der = 0, lastError = 0, lastAngle = 0;

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
  Serial.print(intgForwards);
  Serial.print("\n");
  while (millis() < currTime + period); //Wait once before we actually start so values dont get divided by 0 and nan

}

void loop() {

  currTime = millis();
  count = M1.read(); //get location of motor 1
  count2 = M2.read(); //location motor 2

  if (currTime > 3000 && currTime < 8000 ){
    setDistance = 0;
    setAngle = PI;

  } else if (currTime > 8000) {
    setDistance = 0.3048;
    setAngle = PI;
      //M1Volt = 0;
      //M2Volt = 0;
  } else {
    setDistance = 0;
    setAngle = 0;
  }
 

  currPosition1 = count / CPR * 2 * 3.14159; //Current Position in radians
  currPosition2 = count2 / CPR * 2 * 3.14159; //Current Position in radians
  currVel1 = (currPosition1 - lastPosition1) / ((currTime - lastTime) / 1000) ; //Velocity in rad/sec
  currVel2 = (currPosition2 - lastPosition2) / ((currTime - lastTime) / 1000) ;

  
  lastPosition1 = currPosition1;
  lastPosition2 = currPosition2;
  currAngle = r * (currPosition1 - currPosition2) / d; //Current angle of the robot in radians from the starting point
  currDistance = (currPosition1 + currPosition2) * r / 2;
  
  setRotVel = (setAngle - currAngle) * outerRotP + outerRotD * (setAngle - currAngle - lastError) / ((currTime - lastTime) / 1000); //PD closed loop controller //y[n] = D*N*(u[n]-u[n-1]) + (1-N*Ts)*y[n-1];
  setForwardsVel = (setDistance - currDistance) * outerDistP + outerDistD * (setDistance - currDistance - lastDistance) / ((currTime - lastTime) / 1000);
  if (setRotVel > 4){
    setRotVel = 4;
  }
  if  (setForwardsVel > .75){
    setForwardsVel = .75;
  }
  rotVel = (currVel1 - currVel2) * r / d; //Rotational velocity of the vehicle
  forwardsVel = r * (currVel1 + currVel2) / 2; //Instanteous forwards velocity of the vehicle
  

  
  intgRot += (currTime - lastTime) * ((setRotVel - rotVel) / 1000);
  intgForwards += (currTime - lastTime) * ((setForwardsVel - forwardsVel) / 1000);

  VaBar = (PControlForwards ) * (setForwardsVel - forwardsVel) + intgForwards * IControlForwards;
  VaDelta = (PControlRot ) * (setRotVel - rotVel) + intgRot * IControlRot;



  if (abs((setAngle - currAngle) / setAngle) < .01){
    VaDelta = 0;
  }
  
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
    //Serial.print(M1Volt / VCC * CYCLE);
    //Serial.print("\t");
   // Serial.print(M2Volt / VCC * CYCLE);
    //Serial.print("\t");
    Serial.print(currAngle);
    Serial.print("\t");
    Serial.print(currTime);
    //Serial.print("\t");
    //Serial.print(forwardsVel);
    //Serial.print("\t");
    //Serial.print(currVel2);
    //Serial.print("\t");
    //Serial.print(currTime);
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

  lastAngle = currAngle;
  lastTime = currTime;
  lastError = setAngle - currAngle;
  lastDistance = setDistance - currDistance;
  
}
