#include <ArduinoQueue.h>

#define __movingSelection 5

ArduinoQueue<double> parallelQ(__movingSelection);
double parallelAverage=0.;

void setup() {
  // Set baud rate
  Serial.begin(9600);

  for (int i=1; i <= __movingSelection; i++) {
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
  average=(__movingSelection * average - q.dequeue() + nValue) / __movingSelection;
  q.enqueue(nValue);

  return average;
}

void loop() {
  Serial.println(replaceInAverage(parallelAverage, parallelQ, measureSpeed()));
}
