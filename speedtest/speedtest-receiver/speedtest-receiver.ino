#include <ArduinoQueue.h>

#define __moving 5

ArduinoQueue<double> parallelQ(__moving);
double parallelAverage=0.;

void setup() {
  Serial.begin(9600);
  for (int i=1; i <= __moving; i++) {
    parallelQ.enqueue(measureSpeed());
    parallelAverage += (parallelQ.getTail() - parallelAverage) / i;
  }
}

double measureSpeed() {
  unsigned long start=micros();
  for (int i=0; i < 256; i++) {
    while (((PINB & 3) << 6 | (PIND & 252) >> 2) != i);
  }

  // 255 multiplied by ratio in seconds
  return 255000000.0 / (micros() - start);
}

double replaceInAverage(double &average, ArduinoQueue<double> &q, double nValue) {
  average=(__moving * average - q.dequeue() + nValue) / __moving;
  q.enqueue(nValue);
  return average;
}

void loop() {
  Serial.println(replaceInAverage(parallelAverage, parallelQ, measureSpeed()));
}
