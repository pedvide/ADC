// Teensy adc_pdb_extTrigger_DMA use PDB counter (pin 11) to clock ADC A0
// jumper PWM 23 to pin 11 for clock source or use PDB timer
// https://forum.pjrc.com/threads/24492-Using-the-PDB-on-Teensy-3?p=128046&viewfull=1#post128046
// manitou 12-20-2016 04:03 PM
// ------------------------------------
// Teensy 3.2
// ------------------------------------
// PDB initiated transfer rates approximates
// These numbers do not change much if interrupt 
// services are enabled or disabled or DMA is used
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
  adcval = adc->adc0->readSingle();                // read and clear
}

void adc_init() {
  adc->disablePGA(ADC_0);                                 // no gain
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);       // max reading is 3.3V
  adc->setAveraging(0, ADC_0);                            // no averaging
  adc->setResolution(12, ADC_0);                          // 12 bit adc
  adc->disableCompare(ADC_0);                             // no voltage range to compare input to
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_0);  // Speed to convert to votlage to number
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_0);      // Time to sample signal, short time needs low impedance signal source
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
    adc->adc0->startSingleRead(PIN_ADC);             // call this to setup everything before the lptmr starts, differential is also possible
    adc->enableInterrupts(ADC_0);
    // Set External clock for ADC
    // connect pin 11 to pin 23 for this to work
    analogWriteResolution(4);
    analogWriteFrequency(23,long(Fadc));             // Create clock on pin13
    analogWrite(23, 8);                              // 50% dutycycle clock
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
