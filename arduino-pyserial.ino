uint8_t irn;
uint8_t iri;
uint8_t ir[4][21];

void setup() {
  Serial.begin(9600);
  irn=255;
  iri=0;
  for (uint8_t i=0; i < 4; i++)
    ir[i][20]=0;
  for (uint8_t i=0; i < 20; i++)
    pinMode(i, INPUT);
}

uint8_t readw() {
  if (irn == 255) {
    delay(10);
    return Serial.read();
  }
  return ir[irn][iri++];
}

void pinmode() {
  uint8_t pin=readw();
  pinMode(pin, readw());
}

void pinwrite() {
  uint8_t pin=readw();
  uint8_t value=readw();

  if (readw())
    analogWrite(pin, value);
  else
    digitalWrite(pin, value);
}

void pinread() {
  uint8_t pin=readw();

  if (readw())
    Serial.println(analogRead(pin));
  else
    Serial.println(digitalRead(pin));
}

void delayf() {
  delay((readw() << 8) + readw());
}

void delaymicroseconds() {
  delayMicroseconds((readw() << 8) + readw());
}

void writef() {
  Serial.write(readw());
  Serial.println();
}

void pinchange() {
  uint8_t pin=readw();
  digitalWrite(pin, !digitalRead(pin));
}

void pinclick() {
  uint8_t pin=readw();
  digitalWrite(pin, HIGH);
  delayf();
  digitalWrite(pin, LOW);
}

void echo() {
  uint8_t bcount=readw();
  while (bcount) {
    Serial.print((char)readw());
    bcount--;
  }
  Serial.println();
}

void (*interrupta[])()={
  interrupt0,
  interrupt1,
  interrupt2,
  interrupt3
};

void attachinterrupt() {
  uint8_t pin=readw();
  uint8_t i=readw();
  attachInterrupt(digitalPinToInterrupt(pin), (*interrupta[i]), readw());
}

void detachinterrupt() {
  detachInterrupt(digitalPinToInterrupt(readw()));
}

void setinterrupt() {
  uint8_t bcount=readw();
  uint8_t i=readw();

  Serial.readBytes(ir[i], bcount);
  ir[i][20]=bcount;
}

void runinterrupt() {
  uint8_t i=readw();
  uint8_t times=readw();
  while(times) {
    interrupt(i);
    times--;
  }
}

void (*funcs[])()={
  pinmode,
  pinwrite,
  pinread,
  delayf,
  delaymicroseconds,
  0,
  writef,
  pinchange,
  pinclick,
  echo,
  attachinterrupt,
  detachinterrupt,
  setinterrupt,
  runinterrupt
};

void interrupt(uint8_t i) {
  irn=i;
  while (iri < ir[irn][20])
    (*funcs[readw()])();
  irn=255;
  iri=0;
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
}
