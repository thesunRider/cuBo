

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial1.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println("Write to PC");
Serial1.println("Write to RaspberryPI");
delay(1000);

  if (Serial1.available()) {
    while (Serial1.available()){
    Serial.println("From pi");
    Serial.println((char) Serial1.read());
    }
  }

}



