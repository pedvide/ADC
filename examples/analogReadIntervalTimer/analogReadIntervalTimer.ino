#include "IntervalTimer.h"

volatile bool printNow = true;
volatile uint32_t timerCounter0;
volatile uint32_t timerCounter1;
volatile uint32_t timerCounter2;

void printTimerCounters() {
  Serial.print("timer0: ");
  Serial.print(timerCounter0);
  Serial.print("\t");
  Serial.print("timer1: ");
  Serial.print(timerCounter1);
  Serial.print("\t");
  Serial.print("timer2: ");
  Serial.print(timerCounter2);
  Serial.print("\n\r");
  printNow = false;
}

void timerCallback0() {
  timerCounter0++;
}

void timerCallback1() {
  timerCounter1++;
  timerCounter0 = 0;
}

void timerCallback2() {
  timerCounter2++;
  printNow = true;
}

void setup() {
  Serial.begin(true);
  delay(500);
  IntervalTimer timer0;
  IntervalTimer timer1;
  IntervalTimer timer2;
  timer0.begin(timerCallback0, 500); // 2 kHz
  timer1.begin(timerCallback1, 5000000); // 5 seconds
  timer2.begin(timerCallback2, 1000000); // 1 second
}

void loop() {
  if (printNow) printTimerCounters();
}
