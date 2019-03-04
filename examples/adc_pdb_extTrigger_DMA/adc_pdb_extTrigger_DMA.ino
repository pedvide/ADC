// teensy adc_pdb_extTrigger_DMA use PDB counter (pin 11) to clock ADC A0
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
// ------------------------------------

#define BUFFERSIZE 16384                           // The buffer takes dynamic memory. Stay below 85% of total dynamic memory
#include "ADC.h"
#include <DMAChannel.h>
ADC *adc = new ADC();
DMAChannel dma0;
DMAMEM static uint16_t buf[BUFFERSIZE];            // buffer

#define PIN_ADC 5                                  // ADC input pin
#define PRINTREGISTER(x) Serial.print(#x" 0x"); Serial.println(x,HEX)

// variables used in the interrupt service routines (need to be volatile)
volatile uint32_t pdbticks, adcticks, adcval;
volatile bool     adc0_busy = false;

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

// DMA interrupt service routine
// This is called when the DMA buffer is full
void dma0_isr(void) {
  adc0_busy = false;
  dma0.clearInterrupt();
  dma0.clearComplete();
}

void adc_init() {
  adc->disablePGA(ADC_0);        
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
  adc->setAveraging(0, ADC_0); 
  adc->setResolution(16, ADC_0); 
  adc->disableCompare(ADC_0);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_0);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_0);      

  // Initialize dma
  dma0.source((volatile uint16_t&)ADC0_RA);
  dma0.destinationBuffer(buf, sizeof(buf));
  dma0.triggerAtHardwareEvent(DMAMUX_SOURCE_ADC0);
  dma0.interruptAtCompletion();
  //dma06.disableOnCompletion();                    // Depends on what you want to do after DMA buffer is full
  dma0.attachInterrupt(&dma0_isr);
}

void setup() {
  while (!Serial && millis() < 3000) ;
  Serial.begin(Serial.baud());
  Serial.println("ADC external trigger through PDB with DMA");

  pinMode(PIN_ADC, INPUT);
  delay(1000);
  adc_init();
}

float         delta, expected;
uint32_t      t, cnta=0, cntp=0;
elapsedMicros sinceStart_micros;

void loop() {
  memset((void*)buf, 0, sizeof(buf));
  for (long Fadc = 200000; Fadc < 2000000; Fadc = Fadc+5000) {
    Serial.printf("Freq: %d", Fadc); 
    adc0_busy = true;
    adc->adc0->startExtTrigPDB(true);                // enable external LPTMR trigger and its interrupt
    adc->adc0->startSingleRead(PIN_ADC);             // call this to setup everything before the lptmr starts, differential is also possible
    adc->enableInterrupts(ADC_0);                    // not needed
    adc->enableDMA(ADC_0);                           // set ADC_SC2_DMAEN
    dma0.enable();                                   //
    // Set External clock for ADC
    // connect pin 11 to pin 23 for this to work
    analogWriteResolution(4);
    analogWriteFrequency(23,long(Fadc));             // Create clock on pin13
    analogWrite(23, 8);                              // 50% dutycycle clock
    sinceStart_micros = 0; 
    adcticks = 0;
    pdbticks = 0;
    while (adc0_busy) {    
      // Wait until dma buffer is full and dma initiates interrupt
      // Serial.printf("%d %d\n", adcticks, lptmrticks); 
    }
    t = sinceStart_micros;
    cnta = adcticks; 
    cntp = pdbticks;
    // Stop ADC
    dma0.disable();
    adc->disableDMA(ADC_0);
    adc->adc0->stopExtTrigPDB(true);
    expected = t*1.e-6*Fadc;
    delta = Fadc - (BUFFERSIZE / (t*1.e-6));
    Serial.printf(" ADC %d", cnta); 
    Serial.printf(" PDB %d", cntp); 
    Serial.printf(" off by %+.3f Hz\n", delta);
    delay(100);
  }
}
