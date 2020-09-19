uint8_t irn = 255;
uint8_t iri = 0;
uint8_t ir[2][20];

void setup() {
  Serial.begin(19200);
}

uint8_t readw() {
  if (irn == 255)
    return Serial.read();
  else
    return ir[irn - 2][iri++];
}

void pinmode() {
  uint8_t pin = readw();
  pinMode(pin, readw());
}

void pinwrite() {
  uint8_t pin = readw();
  uint8_t value = readw();;

  if (readw())
    analogWrite(pin, value);
  else
    digitalWrite(pin, value);
}

void pinread() {
  uint8_t pin = readw();

  if (readw())
    Serial.println(analogRead(pin));
  else
    Serial.println(digitalRead(pin));
}

void delayf() {
  delay(readw());
}

void echo() {
  uint8_t cc = readw();
  while (cc) {
    Serial.print((char)readw());
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
  uint8_t bcount = readw();
  uint8_t pin = readw();

  attachInterrupt(digitalPinToInterrupt(pin), (*interrupt[pin - 2]), readw());
  Serial.readBytes(ir[pin - 2], bcount);
}

void detachinterrupt() {
  detachInterrupt(digitalPinToInterrupt(readw()));
}

void delaymicroseconds() {
  delayMicroseconds(readw());
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
  (*funcs[readw()])();
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
