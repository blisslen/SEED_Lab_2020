#include <Wire.h>


//i2c handling
#define ADDRESS 127 //device address
#define HOST 69
#define RESET 0 //reset position command
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

double CYCLE = 255;
int ENABLE = 4; 
int M1DIR = 7, M2DIR = 8, MOTOR1 = 9, MOTOR2 = 10;
int SWITCH = 5;
int STATUS = 12;
double CPR = 1600;
double currPosition = 0, lastPosition = 0, lastTime = 0;
double currVel = 0;  
int A[2] = {0, 0};
int B[2] = {0, 0};
int pinB = 5;
int pinA = 2;
int count = 0;
int startingTime;
int period = 5;
double currTime = 0;
double voltageCommand, destRads, integral = 0;
double VCC = 5;
double PControl = .6512;
double IControl = PControl * .1508;
double pi = 3.14159;

bool turnUp = false;

void setup() {
  Wire.begin(ADDRESS); 
  Wire.onRequest(handleRequest);
  Wire.onReceive(handleCommand);
  
  //Connect a switch between pin 4 and ground
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(ENABLE, OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(STATUS, INPUT);

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);  
  //Initalize interrupt
  attachInterrupt(digitalPinToInterrupt(pinA), changeA, CHANGE);
  
  Serial.begin(115200);
  startingTime = millis();

  digitalWrite(ENABLE, HIGH); //Setting Low disables shield motor outputs
  digitalWrite(M1DIR, HIGH); //HI is CW, LOW is CCW
  analogWrite(MOTOR1, 0 * CYCLE); //1 * CYCLE is Max Speed, 0 * cycle is not moving
}

void loop() {
  currTime = millis();
  if (currTime < 3000){
    destRads = pi / 2;
  } else if (currTime < 6000){
    destRads = pi;
  } else if (currTime < 9000){
    destRads = 3 * pi / 2;
  } else {
    destRads = 2 * pi;
  }
  
  currPosition = count / CPR * 2 * 3.14159; //Current Position in radians
  currVel = (currPosition - lastPosition) / ((currTime - lastTime)/1000) ; //Velocity in rad/sec
  lastPosition = currPosition;
  integral += ((currTime - lastTime) / 1000) * (destRads - currPosition) ;
  voltageCommand = (destRads - currPosition) * PControl + integral * IControl;
  if (voltageCommand > 5) voltageCommand = 5;
  if (voltageCommand <-5) voltageCommand = -5;
  
  if (voltageCommand < 0){
    digitalWrite(M1DIR, LOW);
    voltageCommand *= -1;
  } else {
    digitalWrite(M1DIR, HIGH);
  }


  Serial.print("Destination (radians): ");
  Serial.print(destRads);
  Serial.print("\t Voltage Command (0-1): ");
  Serial.print(voltageCommand / VCC);
  Serial.print("\n");
  
  analogWrite(MOTOR1, voltageCommand / VCC * CYCLE);
  
  while (millis() < currTime + period);  //Sample every 5ms
 
  lastTime = currTime;
}
  
void changeA(){
 if (digitalRead (pinA)){ //If pin A changes low to high
   turnUp = digitalRead (pinB); //Turning CW if A: HL && B High
 } else { //If pin A changes high to low
   turnUp = !digitalRead (pinB); //Turning CCW if A: LH && B Low
 } 
 if (turnUp){
   count++;
 }
 else {
   count--;
 }
}


void handleRequest() {
  while(1 < Wire.available()) { 
    byte* posbytes = (byte*)&currPosition;
    Wire.beginTransmission(HOST);
    for(int i= 0; i < 4; i++) {
      Wire.write(posbytes[i]);
    }
    Wire.endTransmission(HOST);
  }
}

void handleCommand(int n) {
  while(1 < Wire.available()) {
    int cmd = Wire.read();
    switch(cmd) {
      case RESET: 
        currPosition = 0;
        destRads = 0;
        lastPosition = 0;
        break;
      case NORTH:
        destRads = 0;
        break;
      case EAST:
        destRads = PI/2;
        break;
      case SOUTH:
        destRads = PI;
        break;
      case WEST:
        destRads = 3*PI/2;
        break;  
    }
  }
}
