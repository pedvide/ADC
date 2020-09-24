// Teensy external ADC clock example
//
// This program increases external clock and reports if adc conversion were missed.
//
// adc_pdb_extTrigger_DMA uses PDB counter (pin 11) to clock ADC A0
// jumper PWM 23 to pin 11 for clock source or use PDB timer

// ------------------------------------
// Teensy 3.2
// ------------------------------------
// ADC rates:
// Averaging 0 ------------------------
// 16 bit 710kS/s, VERY_HIGH, VERY_HIGH
// 12 bit 830kS/s, VERY_HIGH, VERY_HIGH
//  8 bit 930kS/s, VERY_HIGH, VERY_HIGH
// 16 bit 365kS/s, HIGH, HIGH
// 12 bit 430kS/s, HIGH, HIGH
//  8 bit 480kS/s, HIGH, HIGH
// Averaging 4 ------------------------
// 16 bit 205kS/s, VERY_HIGH, VERY_HIGH
// 12 bit 250kS/s, VERY_HIGH, VERY_HIGH
// 8  bit 290kS/s, VERY_HIGH, VERY_HIGH
// ------------------------------------

#include "ADC.h"
ADC *adc = new ADC();

#define PIN_ADC 5                                  // ADC input pin
#define PRINTREGISTER(x) Serial.print(#x" 0x"); Serial.println(x,HEX)

// variables used in the interrupt service routines (need to be volatile)
volatile uint32_t pdbticks, adcticks, adcval;

// Programmable Delay Block Interrupt Service Routine
void pdb_isr(void)
{
  PDB0_SC = (PDB_SC_TRGSEL(00) | PDB_SC_PDBEN | PDB_SC_PDBIE ) | PDB_SC_LDOK; // (also clears interrupt flag)
  pdbticks++;
}

// ADC interrupt service routine
// Its not needed when DMA is enabled
void adc0_isr() {
  adcticks++;
  adcval = adc->adc0->readSingle();                      // read and clear
}

void adc_init() {
  adc->adc0->disablePGA();                               // no gain
  adc->adc0->setReference(ADC_REFERENCE::REF_3V3);       // max reading is 3.3V
  adc->adc0->setAveraging(0);                            // no averaging
  adc->adc0->setResolution(12);                          // 12 bit adc
  adc->adc0->disableCompare();                           // no voltage range to compare input to
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);  // Speed to convert to votlage to number
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);      // Time to sample signal, short time needs low impedance signal source
}

void setup() {
  while (!Serial && millis() < 3000) ;
  Serial.begin(Serial.baud());
  Serial.println("ADC external trigger through PDB");

  pinMode(PIN_ADC, INPUT);
  delay(1000);
  adc_init();
}

float       delta, expected;
uint32_t    t, cnta=0, cntp=0;
elapsedMicros sinceStart_micros;

void loop() {
  for (long Fadc = 200000; Fadc < 2000000; Fadc = Fadc+5000) {
    Serial.printf("Freq: %d", Fadc); 
    adc->adc0->startExtTrigPDB(true);              // enable external LPTMR trigger and its interrupt
    adc->adc0->startSingleRead(PIN_ADC);           // call this to setup everything before the lptmr starts, differential is also possible
    adc->adc0->enableInterrupts(adc0_isr);
    // Set External clock for ADC
    // connect pin 11 to pin 23 for this to work
    analogWriteResolution(4);                            // Prepare clock
    analogWriteFrequency(23,long(Fadc));                 // Create clock on pin13
    analogWrite(23, 8);                                  // 50% dutycycle clock
    sinceStart_micros = 0; adcticks = 0; pdbticks = 0;
    delay(1000);
    t = sinceStart_micros; cnta = adcticks; cntp = pdbticks;
    // Stop ADC
    adc->adc0->stopExtTrigPDB(true);
    expected = t*1.e-6*Fadc;
    delta = (cnta-expected )*1000000.0/t;
    Serial.printf(" ADC %d", cnta); 
    Serial.printf(" PDB %d", cntp); 
    Serial.printf(" off by %+.3f Hz\n", delta);
  }
}
