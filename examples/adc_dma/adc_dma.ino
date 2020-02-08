/* Example for using DMA with ADC
    This example uses DMA object to do the sampling.  It does not use a timer so it runs
    at whatever speed the ADC will run at with current settings.

    It should work for Teensy LC, 3.x and T4

  DMA: using AnalogBufferDMA with two buffers, this runs in continuous mode and when one buffer fills
    an interrupt is signaled, which sets flag saying it has data, which this test application
    scans the data, and computes things like a minimum, maximum, average values and an RMS value.
    For the RMS it keeps the average from the previous set of data.
*/

#ifdef ADC_USE_DMA

#include <ADC.h>
#include <AnalogBufferDMA.h>


// This version uses both ADC1 and ADC2
#if defined(KINETISL)
const int readPin_adc_0 = A0;
#elif defined(KINETISK)
const int readPin_adc_0 = A0;
const int readPin_adc_1 = A2;
#else
const int readPin_adc_0 = A0;
const int readPin_adc_1 = 26;
#endif

ADC *adc = new ADC(); // adc object
const uint32_t initial_average_value = 2048;

// Going to try two buffers here  using 2 dmaSettings and a DMAChannel
#ifdef KINETISL
const uint32_t buffer_size = 500;
#else
const uint32_t buffer_size = 1600;
#endif

DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff1[buffer_size];
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff2[buffer_size];
AnalogBufferDMA abdma1(dma_adc_buff1, buffer_size, dma_adc_buff2, buffer_size);

#ifdef ADC_DUAL_ADCS
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc2_buff1[buffer_size];
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc2_buff2[buffer_size];
AnalogBufferDMA abdma2(dma_adc2_buff1, buffer_size, dma_adc2_buff2, buffer_size);
#endif

void setup() {
    while (!Serial && millis() < 5000) ;

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin_adc_0, INPUT); //pin 23 single ended
#ifdef ADC_DUAL_ADCS
    pinMode(readPin_adc_1, INPUT);
#endif

    Serial.begin(9600);
    Serial.println("Setup ADC_0");
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->adc0->setAveraging(8); // set number of averages
    adc->adc0->setResolution(12); // set bits of resolution


    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

    // enable DMA and interrupts
    Serial.println("before enableDMA"); Serial.flush();


    // setup a DMA Channel.
    // Now lets see the different things that RingbufferDMA setup for us before
    abdma1.init(adc, ADC_0);
    abdma1.userData(initial_average_value); // save away initial starting average
#ifdef ADC_DUAL_ADCS
    Serial.println("Setup ADC_1");
    adc->adc1->setAveraging(8); // set number of averages
    adc->adc1->setResolution(12); // set bits of resolution
    abdma2.init(adc, ADC_1);
    abdma2.userData(initial_average_value); // save away initial starting average
    adc->adc1->startContinuous(readPin_adc_1);
#endif

    // Start the dma operation..
    adc->adc0->startContinuous(readPin_adc_0);

    Serial.println("End Setup");
}

char c = 0;


void loop() {

    // Maybe only when both have triggered?
#ifdef ADC_DUAL_ADCS
    if ( abdma1.interrupted() && abdma2.interrupted()) {
        if (abdma1.interrupted()) ProcessAnalogData(&abdma1, 0);
        if (abdma2.interrupted()) ProcessAnalogData(&abdma2, 1);
        Serial.println();
    }
#else
    if ( abdma1.interrupted()) {
        ProcessAnalogData(&abdma1, 0);
        Serial.println();
    }
#endif

}

void ProcessAnalogData(AnalogBufferDMA *pabdma, int8_t adc_num) {
  uint32_t sum_values = 0;
  uint16_t min_val = 0xffff;
  uint16_t max_val = 0;

  uint32_t average_value = pabdma->userData();

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
  average_value = sum_values / buffer_size;
  Serial.printf(" %d - %u(%u): %u <= %u <= %u %d ", adc_num, pabdma->interruptCount(), pabdma->interruptDeltaTime(), min_val,
                average_value, max_val, rms);
  pabdma->clearInterrupt();

  pabdma->userData(average_value);
}

#else // make sure the example can run for any boards (automated testing)
void setup() {}
void loop() {}
#endif // ADC_USE_DMA