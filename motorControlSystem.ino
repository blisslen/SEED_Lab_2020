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

bool turnUp = false;

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
  attachInterrupt(digitalPinToInterrupt(pinA), changeA, CHANGE);
  
  Serial.begin(115200);
  startingTime = millis();

  digitalWrite(ENABLE, HIGH); //Setting Low disables shield motor outputs
  digitalWrite(M1DIR, HIGH); //HI is CW, LOW is CCW
  analogWrite(MOTOR1, 0 * CYCLE); //1 * CYCLE is Max Speed, 0 * cycle is not moving
}

void loop() {
  currTime = millis();
  
  currPosition = count / CPR * 2 * 3.14159; //Current Position in radians
  currVel = (currPosition - lastPosition) / ((currTime - lastTime)/1000) ; //Velocity in rad/sec
  lastPosition = currPosition;

  integral += ((currTime - lastTime) / 1000) * (destRads - currPosition) ;

  voltageCommand = (destRads - currPosition) * .32007 + integral * 0.01120245;
  
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
