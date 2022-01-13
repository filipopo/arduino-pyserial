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

void pinmode() {
  //pinmode 2 output
  sendByte(0);
  sendByte(2);
  sendByte(1);

  //pinmode 3 output
  sendByte(0);
  sendByte(3);
  sendByte(1);
}

void pin2on() {
  //pinwrite 2 high digital
  sendByte(1);
  sendByte(2);
  sendByte(1);
  sendByte(0);
}

void pin2off() {
  //pinwrite 2 low digital
  sendByte(1);
  sendByte(2);
  sendByte(0);
  sendByte(0);
}

void changepins() {
  //pinchange 2
  sendByte(7);
  sendByte(2);

  //delay 1000, leading zeros don't change anything but make it more readable
  sendByte(4);
  sendByte(0b00000011);
  sendByte(0b11101000);

  //pinchange 2
  sendByte(7);
  sendByte(2);

  //pinclick 3 1000
  sendByte(8);
  sendByte(3);
  sendByte(0b00000011);
  sendByte(0b11101000);

  //delaymicroseconds 16000
  sendByte(5);
  sendByte(0b00111110);
  sendByte(0b10000000);

  //pinchange 2
  sendByte(7);
  sendByte(2);

  //prevent buffer overflow because of sending delay(s)
  execute = 0;
}

void write() {
  //write 33
  sendByte(6);
  sendByte(33);

  //pinread 2
  sendByte(2);
  sendByte(2);
}

void reset() {
  //pinwrite 4 high digital
  sendByte(1);
  sendByte(4);
  sendByte(1);
  sendByte(0);

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
