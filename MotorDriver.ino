  double CYCLE = 255;
  int ENABLE = 4; 
  int M1DIR = 7, M2DIR = 8, MOTOR1 = 9, MOTOR2 = 10;
  int SWITCH = 5;
  int STATUS = 12;
  

void setup() {
  //Connect a switch between pin 4 and ground
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(ENABLE, OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(STATUS, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  
  if (!digitalRead(STATUS)){//STATUS pin is set by the shield if the motors are not functioning normally
    digitalWrite(ENABLE, HIGH); //Setting Low disables shield motor outputs
    digitalWrite(M1DIR, LOW); //HI is CW, LOW is CCW
    analogWrite(MOTOR1, 1 * CYCLE); //1 * CYCLE is Max Speed, 0 * cycle is not moving
  }
}
