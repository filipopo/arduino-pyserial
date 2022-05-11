#include <ArduinoQueue.h>

#define __movingSelection 5

double serialAverage=0.;
ArduinoQueue<double> serialQ(__movingSelection);
double parallelAverage=0.;
ArduinoQueue<double> parallelQ(__movingSelection);

void setup() {
  // Set baud rate
  Serial.begin(9600);
  
  // Set pin 10 as output
  DDRB |= (1 << 3);

  for (int i=1; i <= __movingSelection; i++) {
    serialQ.enqueue(measureSpeed(1));
    serialAverage += (serialQ.getTail() - serialAverage) / i;

    //parallelQ.push(measureSpeed(0));
    //parallelAverage += (parallelQ.getTail() - parallelAverage) / i;
  }
}

double measureSpeed(char mode) {
  //Serial.write(mode);

  unsigned long start=millis();
  for (int i=0; i < 256; i++) {
    if (mode) {
      while (Serial.read() != i);
    } else {
      PORTB &= ~(1 << 3);
      while (((PINB & 3) << 6 | (PIND & 252) >> 2) != i);
      PORTB |= (1 << 3);
    }
  }

  // 255 multiplied by ratio in seconds
  return 255000.0 / (millis() - start);
}

double replaceInAverage(double &average, ArduinoQueue<double> &q, double newValue) {
  average=(__movingSelection * average - q.dequeue() + newValue) / __movingSelection;
  q.enqueue(newValue);

  return average;
}

void loop() {
  Serial.println(replaceInAverage(serialAverage, serialQ, measureSpeed(1)));
  //Serial.print(' ');
  //Serial.println(replaceInAverage(parallelAverage, parallelQ, measureSpeed(0)));
  /*Serial.write(1);
  Serial.print(1);
  Serial.print(' ');
  Serial.println(2);
  Serial.write(0);*/
}
