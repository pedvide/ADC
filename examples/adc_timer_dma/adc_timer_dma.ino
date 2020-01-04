/* Example for triggering the ADC with Timer using DMA instead of interrupts
    Valid for the current Teensy 3.x and 4.0.


  Timers:
    On Teensy 3.x this uses the PDB timer.

    On Teensy 4, this uses one or two of the unused QTimers.

    Setting it up:  The variables readPin must be defined for a pin that is valid for the first (or only)
    ADC.  If the processor has a second ADC and is enabled, than readPin2 must be configured to be a pin
    that is valid on the second ADC.

  DMA: using AnalogBufferDMA with two buffers, this runs in continuous mode and when one buffer fills
    an interrupt is signaled, which sets flag saying it has data, which this test application
    scans the data, and computes things like a minimum, maximum, average values and an RMS value.
    For the RMS it keeps the average from the previous set of data.
*/


#include <ADC.h>
#include <DMAChannel.h>
#include <AnalogBufferDMA.h>

//#define PRINT_DEBUG_INFO

#ifdef KINETISL
#error "Sorry: This example will not run on Teensy LC"
#endif

// This version uses both ADC1 and ADC2
#ifdef KINETISK
const int readPin_adc_0 = A0;
const int readPin_adc_1 = A2;
#else
const int readPin_adc_0 = A0;
const int readPin_adc_1 = 26;
#endif

ADC *adc = new ADC(); // adc object

extern void dumpDMA_structures(DMABaseClass *dmabc);
elapsedMillis elapsed_sinc_last_display;

// Going to try two buffers here  using 2 dmaSettings and a DMAChannel

const uint32_t buffer_size = 1600;
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff1[buffer_size];
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff2[buffer_size];
AnalogBufferDMA abdma1(dma_adc_buff1, buffer_size, dma_adc_buff2, buffer_size);

#if ADC_NUM_ADCS>1
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff2_1[buffer_size];
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff2_2[buffer_size];
AnalogBufferDMA abdma2(dma_adc_buff2_1, buffer_size, dma_adc_buff2_2, buffer_size);
#endif

void setup() {
  while (!Serial && millis() < 5000) ;

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readPin_adc_0, INPUT); // Not sure this does anything for us
#if ADC_NUM_ADCS>1
  pinMode(readPin_adc_1, INPUT);
#endif
  Serial.begin(9600);
  Serial.println("Setup both ADCs");
  // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
  //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

  // Setup both ADCs
  adc->setAveraging(8); // set number of averages
  adc->setResolution(12); // set bits of resolution
#if ADC_NUM_ADCS>1
  adc->setAveraging(8, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution
#endif

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  // enable DMA and interrupts
  //Serial.println("before enableDMA"); Serial.flush();


  // setup a DMA Channel.
  // Now lets see the different things that RingbufferDMA setup for us before
  abdma1.init(adc, ADC_0/*, DMAMUX_SOURCE_ADC_ETC*/);
#if ADC_NUM_ADCS>1
  abdma2.init(adc, ADC_1/*, DMAMUX_SOURCE_ADC_ETC*/);
#endif
  //Serial.println("After enableDMA"); Serial.flush();

  // Start the dma operation..
  adc->adc0->startSingleRead(readPin_adc_0); // call this to setup everything before the Timer starts, differential is also possible
  adc->adc0->startTimer(3000); //frequency in Hz

  // Start the dma operation..
#if ADC_NUM_ADCS>1
  adc->adc1->startSingleRead(readPin_adc_1); // call this to setup everything before the Timer starts, differential is also possible
  adc->adc1->startTimer(3000); //frequency in Hz
#endif

  print_debug_information();

  Serial.println("End Setup");
  elapsed_sinc_last_display = 0;
}

char c = 0;

int average_value = 2048;

void loop() {

  // Maybe only when both have triggered?
#if ADC_NUM_ADCS>1
  if ( abdma1.interrupted() && (abdma2.interrupted())) {
    if ( abdma1.interrupted()) {
      ProcessAnalogData(&abdma1, 0);
    }
    if ( abdma2.interrupted()) {
      ProcessAnalogData(&abdma2, 1);
    }
    Serial.println();
    elapsed_sinc_last_display = 0;
  }
#else
  if ( abdma1.interrupted()) {
    ProcessAnalogData(&abdma1, 0);
    Serial.println();
    elapsed_sinc_last_display = 0;
  }
#endif
  if (elapsed_sinc_last_display > 5000) {
    // Nothing in 5 seconds, show a heart beat.
    digitalWriteFast(13, HIGH);
    delay(250);
    digitalWriteFast(13, LOW);
    delay(250);
    digitalWriteFast(13, HIGH);
    delay(250);
    digitalWriteFast(13, LOW);
    elapsed_sinc_last_display = 0;
  }
}

void ProcessAnalogData(AnalogBufferDMA *pabdma, int8_t adc_num) {
  uint32_t sum_values = 0;
  uint16_t min_val = 0xffff;
  uint16_t max_val = 0;

  volatile uint16_t *pbuffer = pabdma->bufferLastISRFilled();
  volatile uint16_t *end_pbuffer = pbuffer + pabdma->bufferCountLastISRFilled();

  float sum_delta_sq = 0.0;
  if ((uint32_t)pbuffer >= 0x20200000u)  arm_dcache_delete((void*)pbuffer, sizeof(dma_adc_buff1));
  while (pbuffer < end_pbuffer) {
    if (*pbuffer < min_val) min_val = *pbuffer;
    if (*pbuffer > max_val) max_val = *pbuffer;
    sum_values += *pbuffer;
    int delta_from_center = (int) * pbuffer - average_value;
    sum_delta_sq += delta_from_center * delta_from_center;

    pbuffer++;
  }

  int rms = sqrt(sum_delta_sq / buffer_size);
  Serial.printf(" %d - %u(%u): %u <= %u <= %u %d ", adc_num, pabdma->interruptCount(), pabdma->interruptDeltaTime(), min_val,
                sum_values / buffer_size, max_val, rms);
  pabdma->clearInterrupt();
}

void print_debug_information()
{
#ifdef PRINT_DEBUG_INFO
  // Lets again try dumping lots of data.
  Serial.println("\n*** DMA structures for ADC_0 ***");
  dumpDMA_structures(&(abdma1._dmachannel_adc));
  dumpDMA_structures(&(abdma1._dmasettings_adc[0]));
  dumpDMA_structures(&(abdma1._dmasettings_adc[1]));
  Serial.println("\n*** DMA structures for ADC_1 ***");
  dumpDMA_structures(&(abdma2._dmachannel_adc));
  dumpDMA_structures(&(abdma2._dmasettings_adc[0]));
  dumpDMA_structures(&(abdma2._dmasettings_adc[1]));

#if defined(__IMXRT1062__)

  Serial.println("\n*** ADC and ADC_ETC ***");
  Serial.printf("ADC1: HC0:%x HS:%x CFG:%x GC:%x GS:%x\n", IMXRT_ADC1.HC0, IMXRT_ADC1.HS,  IMXRT_ADC1.CFG, IMXRT_ADC1.GC, IMXRT_ADC1.GS);
  Serial.printf("ADC2: HC0:%x HS:%x CFG:%x GC:%x GS:%x\n", IMXRT_ADC2.HC0, IMXRT_ADC2.HS,  IMXRT_ADC2.CFG, IMXRT_ADC2.GC, IMXRT_ADC2.GS);
  Serial.printf("ADC_ETC: CTRL:%x DONE0_1:%x DONE2_ERR:%x DMA: %x\n", IMXRT_ADC_ETC.CTRL,
                IMXRT_ADC_ETC.DONE0_1_IRQ, IMXRT_ADC_ETC.DONE2_ERR_IRQ, IMXRT_ADC_ETC.DMA_CTRL);
  for (uint8_t trig = 0; trig < 8; trig++) {
    Serial.printf("    TRIG[%d] CTRL: %x CHAIN_1_0:%x\n",
                  trig, IMXRT_ADC_ETC.TRIG[trig].CTRL, IMXRT_ADC_ETC.TRIG[trig].CHAIN_1_0);
  }
#endif
#endif
}

#ifdef PRINT_DEBUG_INFO
void dumpDMA_structures(DMABaseClass *dmabc)
{
  Serial.printf("%x %x:", (uint32_t)dmabc, (uint32_t)dmabc->TCD);

  Serial.printf("SA:%x SO:%d AT:%x NB:%x SL:%d DA:%x DO: %d CI:%x DL:%x CS:%x BI:%x\n", (uint32_t)dmabc->TCD->SADDR,
                dmabc->TCD->SOFF, dmabc->TCD->ATTR, dmabc->TCD->NBYTES, dmabc->TCD->SLAST, (uint32_t)dmabc->TCD->DADDR,
                dmabc->TCD->DOFF, dmabc->TCD->CITER, dmabc->TCD->DLASTSGA, dmabc->TCD->CSR, dmabc->TCD->BITER);
}
#endif