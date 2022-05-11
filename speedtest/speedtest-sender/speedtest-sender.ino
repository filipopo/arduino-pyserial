void setup() {
  // Set baud rate
  Serial.begin(9600);

  // Set Port B (pins 8 and 9) as output, pin 10 is defaulted to input
  DDRB=0b11;

  // Set Port D (pins 2 - 7) as output
  DDRD |= 0b11111100;

}

void loop() {
  //if (Serial.read() == 1) {
    for (int i=0; i < 256; i++)
      Serial.write(i);
  /*} else if (Serial.read() == 0) {
    for (int i=0; i < 256; i++) {
      PORTB=(i & 192) >> 6;
      PORTD=(i & 63) << 2;
      delay(1);
      //Serial.println((PINB & 3) << 6 | (PIND & 252) >> 2));
      while (PINB >> 3 & 1 == 0);
    }
  }*/
}
