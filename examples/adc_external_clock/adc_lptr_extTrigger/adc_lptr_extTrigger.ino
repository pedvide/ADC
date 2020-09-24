// Teensy external ADC clock example 
//
// This program increases external clock and reports if adc conversion were missed.
//
// Using LPTMR hardwre module
// adclptmr uses LPTMR0 counter (pin 13) to clock ADC A0
// jumper PWM pin 23 to pin 13 for clock source or use PDB timer

// ------------------------------------
// Teensy 3.2
// ------------------------------------
//ADCr rates
// Averaging 0 ------------------------
// 16 bit 735kS/s, VERY_HIGH, VERY_HIGH
// 12 bit 835kS/s, VERY_HIGH, VERY_HIGH
//  8 bit 975kS/s, VERY_HIGH, VERY_HIGH
// 16 bit 365kS/s, HIGH16, HIGH
// 16 bit 365kS/s, HIGH, HIGH
// 12 bit 430kS/s, HIGH, HIGH
//  8 bit 480kS/s, HIGH, HIGH
// Averaging 4 ------------------------
// 16 bit 205kS/s, VERY_HIGH, VERY_HIGH
// 12 bit 255kS/s, VERY_HIGH, VERY_HIGH
// 8  bit 290kS/s, VERY_HIGH, VERY_HIGH
// ------------------------------------

#include "ADC.h"
ADC *adc = new ADC();

#define PIN_ADC 5                                                     // A0, Pin 14
#define PRINTREGISTER(x) Serial.print(#x" 0x"); Serial.println(x,HEX)

volatile uint32_t lptmrticks, adcticks, adcval;

void lptmr_isr(void)
{
  LPTMR0_CSR |=  LPTMR_CSR_TCF; // clear
  lptmrticks++;
}

void adc0_isr() {
  adcticks++;
  adcval = adc->adc0->readSingle();
}

void adc_init() {
  adc->adc0->disablePGA();        
  adc->adc0->setReference(ADC_REFERENCE::REF_3V3);
  adc->adc0->setAveraging(0); 
  adc->adc0->setResolution(8); 
  adc->adc0->disableCompare();
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);      
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(PIN_ADC, INPUT);
  pinMode(23, OUTPUT);
  delay(1000);
  adc_init();
}

float    delta, expected;
uint32_t t, cnta=0, cntl=0;
elapsedMicros sinceStart_micros;

void loop() {
  for (long Fadc = 500000; Fadc < 2000000; Fadc = Fadc+5000) {
    adc->adc0->startExtTrigLPTMR(true);              // enable external LPTMR trigger and its interrupt
    adc->adc0->startSingleRead(PIN_ADC);             // call this to setup everything before the pdb starts, differential is also possible
    adc->adc0->enableInterrupts(adc0_isr);
    analogWriteResolution(4);                        // Setup trigger clock
    analogWriteFrequency(23,long(Fadc*2));           // Create clock on pin13
    analogWrite(23, 8);                              // 50% dutycycle clock
    sinceStart_micros = 0; 
    adcticks = 0;
    lptmrticks = 0;
    delay(1000);
    t=sinceStart_micros, cnta=adcticks; cntl= lptmrticks;
    adc->adc0->stopExtTrigLPTMR(true);              // stop external LPTMR trigger and its interrupt
    expected = t*1.e-6*Fadc;
    delta = (cnta-expected)*1000000.0/t;
    Serial.printf("Freq: %d", Fadc); 
    Serial.printf(" ADC %d", cnta); 
    Serial.printf(" LPT %d", cntl); 
    Serial.printf(" off by %+.3f Hz\n", delta);
  }
}
