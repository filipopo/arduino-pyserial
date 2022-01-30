#define USART_BAUDRATE 9600
#define UBRR_VALUE ((F_CPU / (USART_BAUDRATE * 16UL)) - 1)

uint8_t cc = 255;
bool execute = 0;

void setup() {
  // Set baud rate
  UBRR0 = UBRR_VALUE;

  // Set frame format to 8 data bits and default to no parity, 1 stop bit
  UCSR0C |= (3 << UCSZ00);

  // Enable USART reception and transmission and RC complete interrupt
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

  // Set pins 2 and 3 as input_pullup
  PORTD |= (1 << 2) | (1 << 3);

  // Set pin 4 as output
  DDRD |= (1 << 4);

  // Enable INT0 and INT1 interrupts
  EIMSK |= (1 << INT0) | (1 << INT1);

  // Generate INT0 and INT1 interrupts by falling edge
  EICRA |= (1 << ISC01) | (1 << ISC11);
}

void sendByte(uint8_t data) {
  // Wait while previous byte is completed
  while (!(UCSR0A & (1 << UDRE0))) {};

  // Transmit data
  UDR0 = data;
}

void sendBytes(uint8_t *data, uint8_t len) {
  for (uint8_t i = 0; i < len; i++)
    sendByte(data[i]);
}

void pinmode() {
  //pinmode 2 output
  uint8_t data = {0, 2, 1};
  sendBytes(data, 3);

  //pinmode 3 output
  data = {0, 3, 1};
  sendBytes(data, 3);
}

void pin2on() {
  //pinwrite 2 high digital
  uint8_t data = {1, 2, 1, 0};
  sendBytes(data, 4);
}

void pin2off() {
  //pinwrite 2 low digital
  uint8_t data = {1, 2, 0, 0};
  sendBytes(data, 4);
}

void changepins() {
  //pinchange 2
  uint8_t data = {7, 2};
  sendBytes(data, 2);

  //delay 1000, leading zeros don't change anything but make it more readable
  data = {4, 0b00000011, 0b11101000};
  sendBytes(data, 3);

  //pinchange 2
  data = {7, 2};
  sendBytes(data, 2);

  //pinclick 3 1000
  data = {8, 3, 0b00000011, 0b11101000);
  sendBytes(data, 4);

  //delaymicroseconds 16000
  data = {5, 0b00111110, 0b10000000);
  sendBytes(data, 3);

  //pinchange 2
  data = {7, 2};
  sendBytes(data, 2);

  //prevent buffer overflow because of sending delay(s)
  execute = 0;
}

void write() {
  //write 33
  uint8_t data = {6, 33};
  sendBytes(data, 2);

  //pinread 2
  data = {2, 2};
  sendBytes(data, 2);
}

void reset() {
  //pinwrite 4 high digital
  uint8_t data = {1, 4, 1, 0};
  sendBytes(data, 4);

  //reset
  sendByte(5);
}

void (*commands[])() = {
  pinmode,
  pin2on,
  pin2off,
  changepins,
  write,
  reset
};

void loop() {
  // Set pin 4 of port D to low logical state
  PORTD &= (0 << 4);
  delayMicroseconds(100);
  // Set pin 4 of port D to high logical state
  PORTD |= (1 << 4);
  if (execute)
    (*commands[cc])();
  delay(2);
}

// INT0 interrupt service routine
ISR(INT0_vect) {
  // prevent segmentation fault
  if (cc < 4)
    cc++;
  execute = 1;
}

// INT1 interrupt service routine
ISR(INT1_vect) {
  execute = 1;
}

// RX Complete interrupt service routine
ISR(USART_RX_vect) {
  if (UDR0) {
    cc = 255;
    execute = 0;
  }
}
