#include <Wire.h>
#include <Encoder.h>

#define ADDRESS 127
#define RESET 0
#define GIVE_POS 0

uint32_t pos;
Encoder Wheel(2,4);

void setup() {
  Wire.begin(ADDRESS); 
  Wire.onRequest(handleRequest);
  Wire.onReceive(handleCommand);

  Serial.begin(9600);

  pos = 0;

}

void loop() {
  Serial.println(Wheel.read());
  delay(100);
}

void handleRequest() {
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
