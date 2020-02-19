#include <Wire.h>
#include <Encoder.h>

//i2c handling
#define ADDRESS 127 //device address
#define RESET 0 //reset position command

//wheel schema
#define CYCLE 0xFF //PWM max
#define ENABLE 4 //Motor enable pin (RESERVED)
#define M1DIR 7 //Motor 1 direction
#define M2DIR 8 //Motor 2 direction
#define MOTOR1 9
#define MOTOR2 10
#define SWITCH 5 //switch enable for motor driver
#define STATUS 12 //status pin for motor driver
#define CPR 1600 //counts per rotation
#define ENC1 2 //encoder pin 1
#define ENC2 5 //encoder pin 2

//PID control
#define PCONST 0.32007 
#define ICONST 0.01120245
#define DCONST 0

//timing
#define PERIOD 5 //millisecond delay for 

double currPosition = 0, lastPosition = 0, lastTime = 0; //track historical positions and times for PID control
double currVel = 0;  //as well as velocity
// int A[2] = {0, 0};
// int B[2] = {0, 0};
// int count = 0;
int startingTime;
int period = 5;
double currTime = 0;
double voltageCommand, destRads, integral = 0;

Encoder Wheel(ENC1,ENC2);

void setup() {
  Wire.begin(ADDRESS); 
  Wire.onRequest(handleRequest);
  Wire.onReceive(handleCommand);

  Serial.begin(9600);

}

void loop() {
//   Serial.println(Wheel.read());
//   delay(100);
  
  currTime = millis();
  
  currPosition = Wheel.read() / CPR * 2 * 3.14159; //Current Position in radians
  currVel = (currPosition - lastPosition) / ((currTime - lastTime)/1000) ; //Velocity in rad/sec
  lastPosition = currPosition;

  integral += ((currTime - lastTime) / 1000) * (destRads - currPosition) ;

  voltageCommand = (destRads - currPosition) * PCONST + integral * ICONST;
  
  Serial.print(voltageCommand);
  Serial.print("\t");
  
  if (voltageCommand < 0){
    digitalWrite(M1DIR, LOW);
    voltageCommand *= -1;
  } else {
    digitalWrite(M1DIR, HIGH);
  }

  //analogWrite(MOTOR1, voltageCommand * CYCLE);
  
  if (currTime - startingTime < 2000 && currTime - startingTime > 1000){
    Serial.print(currPosition);
    Serial.print("\t");
    Serial.print(currVel);
    Serial.print("\t");
    Serial.print(currTime);
    Serial.print("\t");
    Serial.print(voltageCommand);
    Serial.print("\n");
  }
  
  //Serial.print(count);
  
  while (millis() < currTime + period);  //Sample every 5ms
    
  lastTime = currTime;
}

void handleRequest() {
  long long int pos = 0;
  while(1 < Wire.available()) { 
    pos = Wheel.read();
    Wire.beginTransmission(69);
    Wire.write((byte) (pos>>24));
    Wire.write((byte) (pos>>16));
    Wire.write((byte) (pos>>8));
    Wire.write((byte) (pos>>0));
    Wire.endTransmission(69);
  }
}

void handleCommand(int n) {
  while(1 < Wire.available()) {
    int cmd = Wire.read();
    switch(cmd) {
      case RESET: 
        Wheel.write(0);
        break;
    }
  }
}
