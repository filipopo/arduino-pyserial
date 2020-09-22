uint8_t irn = 255;
uint8_t iri = 0;
uint8_t ir[4][21];

void setup() {
  Serial.begin(19200);
}

uint8_t readw() {
  if (irn == 255)
    return Serial.read();
  else
    return ir[irn][iri++];
}

void pinmode() {
  pinMode(readw(), readw());
}

void pinwrite() {
  uint8_t pin = readw();
  uint8_t value = readw();

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
  delay((readw() << 8) + readw());
}

void echo() {
  uint8_t bcount = readw();
  while (bcount) {
    Serial.print((char)readw());
    bcount--;
  }
  Serial.println();
}

void (*interrupta[])() = {
  interrupt0,
  interrupt1,
  interrupt2,
  interrupt3
};

void attachinterrupt() {
  attachInterrupt(digitalPinToInterrupt(readw()), (*interrupta[readw()]), readw());
}

void detachinterrupt() {
  detachInterrupt(digitalPinToInterrupt(readw()));
}

void delaymicroseconds() {
  delayMicroseconds((readw() << 8) + readw());
}

void writef() {
  Serial.write(readw());
}

void setinterrupt() {
  uint8_t bcount = readw();
  uint8_t i = readw();

  Serial.readBytes(ir[i], bcount);
  ir[i][20] = bcount;
}

void runinterrupt() {
  uint8_t i = readw();
  if (readw())
    while(1)
      interrupt(i);
  else
    interrupt(i);
}

void (*funcs[])() = {
  pinmode,
  pinwrite,
  pinread,
  delayf,
  echo,
  0,
  attachinterrupt,
  detachinterrupt,
  delaymicroseconds,
  writef,
  setinterrupt,
  runinterrupt
};

void interrupt(uint8_t i) {
  irn = i;
  while (iri < ir[irn][20])
    (*funcs[readw()])();
  irn = 255;
  iri = 0;
}

void interrupt0() {
  interrupt(0);
}

void interrupt1() {
  interrupt(1);
}

void interrupt2() {
  interrupt(2);
}

void interrupt3() {
  interrupt(3);
}

void loop() {
  if (Serial.available())
    (*funcs[readw()])();
  delay(50);
}
