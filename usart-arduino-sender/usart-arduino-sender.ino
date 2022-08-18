#define UBRR_VALUE ((F_CPU / (9600 * 16UL)) - 1)

uint8_t cc=255;
bool execute=0;

void setup() {
  // Set baud rate
  UBRR0=UBRR_VALUE;

  // Defaults to frame format of 8 data bits, no parity, 1 stop bit in asynchronous mode
  //UCSR0C |= (3 << UCSZ00);

  // Enable USART transmissions
  UCSR0B |= (1 << TXEN0);

  // Set pins 2 and 3 as input_pullup
  PORTD |= (1 << 2) | (1 << 3);

  // Set pin 4 as output
  DDRD |= (1 << 4);

  // Enable INT0 and INT1 interrupts
  EIMSK |= (1 << INT0) | (1 << INT1);

  // Generate INT0 and INT1 interrupts by falling edge
  EICRA |= (1 << ISC01) | (1 << ISC11);
}

void pinmode() {
  //pinmode 2 output
  uint8_t data[]={0, 2, 1};
  Serial.write(data, 3);

  //pinmode 3 output
  uint8_t data2[]={0, 3, 1};
  Serial.write(data2, 3);

  execute=0;
}

void pin2on() {
  //pinwrite 2 high digital
  uint8_t data[]={1, 2, 1, 0};
  Serial.write(data, 4);
}

void pin2off() {
  //pinwrite 2 low digital
  uint8_t data[]={1, 2, 0, 0};
  Serial.write(data, 4);
}

void changepins() {
  //pinchange 2
  uint8_t data[]={7, 2};
  Serial.write(data, 2);

  //delay 1000, leading zeros don't change anything but make it more readable
  uint8_t data2[]={4, 0b00000011, 0b11101000};
  Serial.write(data2, 3);

  //pinchange 2
  uint8_t data3[]={7, 2};
  Serial.write(data3, 2);

  //pinclick 3 1000
  uint8_t data4[]={8, 3, 0b00000011, 0b11101000);
  Serial.write(data4, 4);

  //delaymicroseconds 16000
  uint8_t data5[]={4, 0b00111110, 0b10000000);
  Serial.write(data5, 3);

  //pinchange 2
  uint8_t data6[]={7, 2};
  Serial.write(data6, 2);

  //prevent buffer overflow because of sending delay(s)
  execute=0;
}

void writef() {
  //write 33
  uint8_t data[]={6, 33};
  Serial.write(data, 2);

  //pinread 2
  uint8_t data2[]={2, 2};
  Serial.write(data2, 2);
}

void reset() {
  cc=255;
  execute=0;

  //reset
  Serial.write(5);
}

void (*commands[])()={
  pinmode,
  pin2on,
  pin2off,
  changepins,
  writef,
  reset
};

void loop() {
  // Set pin 4 of port D to low logical state
  PORTD &= ~(1 << 4);
  delayMicroseconds(100);
  // Set pin 4 of port D to high logical state
  PORTD |= (1 << 4);

  if (execute)
    (*commands[cc])();
  delay(10);
}

// INT0 interrupt service routine which will execute the next command
ISR(INT0_vect) {
  // prevent segmentation fault
  if (cc < 5 || cc == 255)
    cc++;
  execute=1;
}

// INT1 interrupt service routine which will repeat the current command
ISR(INT1_vect) {
  // prevent segmentation fault
  if (cc != 255)
    execute=1;
}
