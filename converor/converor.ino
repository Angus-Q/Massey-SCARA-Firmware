

const int stepPin = 4; //pulse
const int dirPin = 2;
const int enPin = 15;


void setup() {
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,HIGH);
  
}

void loop() {
//  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
//  for(int x = 0; x < 1700; x++) {
//    digitalWrite(stepPin,HIGH); 
//    delayMicroseconds(120); 
//    digitalWrite(stepPin,LOW); 
//    delayMicroseconds(120); 
//  }
//  delay(50); // One second delay
  digitalWrite(dirPin,LOW); //Changes the direction of rotation
  for(int x = 0; x < 10; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(90);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(90);
  }
//  delay(50); 
}
