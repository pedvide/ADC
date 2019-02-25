// teensy adclptmr  use LPTMR0 counter (pin 13) to clock ADC A0
// jumper PWM 23 to pin 13 for clock source or use PDB timer
// https://forum.pjrc.com/threads/40782-LPTMR-on-the-Teensy-3-1-3-2-3-5-3-6

// ------------------------------------
// Teensy 3.2
// ------------------------------------
// LPTMR initiated transfer rates approximates
// These numbers do not change much if interrupt 
// services are enabled or disabled or DMA is used
// Averaging 0 ------------------------
// 16 bit 735kS/s, VERY_HIGH, VERY_HIGH
// 12 bit 870kS/s, VERY_HIGH, VERY_HIGH
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

void lptmr_isr(void)                                                  //
{
  LPTMR0_CSR |=  LPTMR_CSR_TCF;                                       // clear
  lptmrticks++;
}

void adc0_isr() {
  adcticks++;
  adcval = adc->adc0->readSingle();
}

void adc_init() {
  adc->disablePGA(ADC_0);        
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
  adc->setAveraging(0, ADC_0); 
  adc->setResolution(12, ADC_0); 
  adc->disableCompare(ADC_0);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_0);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_0);      
}


void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(PIN_ADC, INPUT);
  delay(1000);
  adc_init();
}

float    delta, expected;
uint32_t t, cnta=0, cntl=0;
elapsedMicros sinceStart_micros;

void loop() {
  for (long Fadc = 500000; Fadc < 2000000; Fadc = Fadc+5000) {
    adc->adc0->startExtTrigLPTMR(true);              // enable external LPTMR trigger and its interrupt
    adc->adc0->startSingleRead(PIN_ADC); // call this to setup everything before the pdb starts, differential is also possible
    adc->enableInterrupts(ADC_0);
    analogWriteResolution(4);
    analogWriteFrequency(23,long(Fadc*2));                               // Create clock on pin13
    analogWrite(23, 8);                                                  // 50% dutycycle clock
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
