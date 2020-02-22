#include <Encoder.h>

double CYCLE = 255;
int ENABLE = 4; 
int M1DIR = 7, M2DIR = 8, MOTOR1 = 9, MOTOR2 = 10;
int SWITCH = 5;
int STATUS = 12;
double CPR = 3200;
double currPosition = 0, lastPosition = 0, lastTime = 0;
double currVel = 0;  
int A[2] = {0, 0};
int B[2] = {0, 0};
int pinB = 6;
int pinA = 2;
int count = 0;
int startingTime;
int period = 5;
double currTime = 0;
double voltageCommand, destRads, integral = 0;
double VCC = 5;
double PControl =  0.716399306583056; // 0.96089093563049; // 
double IControl =   0.0550545955322287; //0.0973447459705228; //
double toWrite = 0;

double pi = 3.14159, derivative = 0;

bool turnUp = false;

Encoder M1(pinA, pinB);

void setup() {
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

  
  Serial.begin(115200);
  startingTime = millis();

  digitalWrite(ENABLE, HIGH); //Setting Low disables shield motor outputs
  digitalWrite(M1DIR, HIGH); //HI is CW, LOW is CCW
  analogWrite(MOTOR1, 0 * CYCLE); //1 * CYCLE is Max Speed, 0 * cycle is not moving
}

void loop() {
  currTime = millis();
  /*
  if (currTime < 3000){
    destRads = pi / 2;
  } else if (currTime < 6000){
    destRads = pi;
  } else if (currTime < 9000){
    destRads = 3 * pi / 2;
  } else {
    destRads = 2 * pi;
  }
  */
  
  if (currTime > 1000){
      destRads = 1;
  }


  count = M1.read();
  //Serial.print(count);
  //Serial.print("\n");
  
  currPosition = count / CPR * 2 * 3.14159; //Current Position in radians
  currVel = (currPosition - lastPosition) / ((currTime - lastTime)/1000) ; //Velocity in rad/sec
  lastPosition = currPosition;
  integral += ((currTime - lastTime) / 1000) * (destRads - currPosition) ;
  //derivative = (currPosition - lastPosition)/((currTime - lastTime)/1000);
  voltageCommand = (destRads - currPosition) * PControl + integral * IControl * (destRads - currPosition); //+ (derivative * N * DControl) / (derivative + N);
  if (voltageCommand > 5) voltageCommand = 5;
  if (voltageCommand <-5) voltageCommand = -5;
  
  if (voltageCommand < 0){
    digitalWrite(M1DIR, LOW);
    voltageCommand *= -1;
  } else {
    digitalWrite(M1DIR, HIGH);
  }
  toWrite = voltageCommand / VCC * CYCLE;
  if (toWrite > 1 && toWrite < 18){
    toWrite = 18;
  }

  if (currTime > 1000 && currTime < 2500){
  //Serial.print("Destination (radians): ");
  //Serial.print(destRads);
  //Serial.print("\tCurrent Pos: ");
  Serial.print(currPosition);
  Serial.print("\t");
  Serial.print(currVel);
  Serial.print("\t");
  Serial.print(currTime);
  Serial.print("\n");
  //Serial.print("\t Motor Command (0-155): ");
  //Serial.print(toWrite);
  //Serial.print("\n");
  }
  
  analogWrite(MOTOR1, toWrite);
  
  while (millis() < currTime + period);  //Sample every 5ms
 
  lastTime = currTime;
}
