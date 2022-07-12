void setup() {
  // Set Port B (pins 8 and 9) as output
  DDRB=0b11;

  // Set Port D (pins 2 - 7) as output
  DDRD=0b11111100;
}

void loop() {
  for (int i=0; i < 256; i++) {
    PORTB=(i & 192) >> 6;
    PORTD=(i & 63) << 2;
  }
}
