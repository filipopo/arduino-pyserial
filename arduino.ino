uint8_t cc; // command
uint8_t irn = 255;
uint8_t iri = 0;
uint8_t ir[2][16];

void setup() {
  Serial.begin(19200);
}

void readw() {
  if (irn == 255)
    cc = Serial.read();
  else
    cc = ir[irn - 2][iri++];
}

void pinmode() {
  readw();
  uint8_t pin = cc;

  readw();
  pinMode(pin, cc);
}

void pinwrite() {
  readw();
  uint8_t pin = cc;

  readw();
  uint8_t value = cc;

  readw();
  if (cc)
    analogWrite(pin, value);
  else
    digitalWrite(pin, value);
}

void pinread() {
  readw();
  uint8_t pin = cc;

  readw();
  if (cc)
    Serial.println(analogRead(pin));
  else
    Serial.println(digitalRead(pin));
}

void delayf() {
  readw();
  delay(cc);
}

void echo() {
  readw();
  while (cc) {
    Serial.print((char)Serial.read());
    cc--;
  }
  Serial.println();
}

void (*reset)() = 0;

void (*interrupt[])() = {
  interrupt2,
  interrupt3
};

void attachinterrupt() {
  readw();
  uint8_t bcount = cc;

  readw();
  uint8_t pin = cc;

  readw();
  attachInterrupt(digitalPinToInterrupt(pin), (*interrupt[pin - 2]), cc);

  Serial.readBytes(ir[pin - 2], bcount);
}

void detachinterrupt() {
  readw();
  detachInterrupt(digitalPinToInterrupt(cc));
}

void delaymicroseconds() {
  readw();
  delayMicroseconds(cc);
}

void (*funcs[])() = {
  pinmode,
  pinwrite,
  pinread,
  delayf,
  echo,
  reset,
  attachinterrupt,
  detachinterrupt,
  delaymicroseconds
};

void command() {
  readw();
  (*funcs[cc])();
  irn = 255;
  iri = 0;
}

void interrupt2() {
  irn = 2;
  command();
}

void interrupt3() {
  irn = 3;
  command();
}

void loop() {
  if (Serial.available())
    command();
  delay(50);
}
