void setup() {
  Serial.begin(9600); 
}

void loop() {
  int y1 = analogRead(A0);
  Serial.println(y1); // the last value is followed by a carriage return and a newline characters.

  delay(10);
}
