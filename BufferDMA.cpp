#include "Arduino.h"
#include "BufferDMA.h"
#include "ADC.h"

DMAChannel *BufferDMA::dmaChannel = nullptr;
volatile int16_t *BufferDMA::p_elems = nullptr;
uint16_t BufferDMA::b_size = 0;
uint8_t BufferDMA::adc_number = 0;
volatile uint32_t *BufferDMA::adc_RA = nullptr;
volatile bool BufferDMA::first_half = false;
volatile int8_t BufferDMA::data_ready = 0;

// Constructor
BufferDMA::BufferDMA(volatile int16_t *buffer, uint32_t len, uint8_t adc_num)
{
    // unfortunately since member functions cannot be used as ISRs, it is not 
    // possible to use multiple instances of BufferDMA
    if (!dmaChannel)
        dmaChannel = new DMAChannel();
    else
        dmaChannel->disable();

    p_elems = buffer;
    b_size = len;
    adc_number = adc_num;
    adc_RA = &ADC0_RA + (uint32_t)0x20000 * adc_number;
    data_ready = 0;
    first_half = false;
}

void BufferDMA::start()
{

    dmaChannel->source(*adc_RA);

    dmaChannel->destinationBuffer((uint16_t *)p_elems, sizeof(uint16_t) * b_size);

    dmaChannel->transferSize(sizeof(uint16_t));

    // enable interrupt at half and completion
    dmaChannel->attachInterrupt(dma_half_complete_isr);
    dmaChannel->interruptAtHalf();
    dmaChannel->interruptAtCompletion();

    uint8_t DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC0;

#if ADC_NUM_ADCS >= 2
    if (adc_number == 1)
    {
        DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC1;
    }
#endif
    dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_ADC); // start DMA channel when ADC finishes a conversion

    uint32_t temp = *adc_RA; // clear ADC completion bit before enabling DMA

    dmaChannel->enable();
}

BufferDMA::~BufferDMA()
{
    dmaChannel->detachInterrupt();
    dmaChannel->disable();
    delete dmaChannel;
    dmaChannel = nullptr;
}

void dma_half_complete_isr()
{
    BufferDMA::dmaChannel->clearInterrupt();
    BufferDMA::data_ready++;
    BufferDMA::first_half = !BufferDMA::first_half;
}

int8_t BufferDMA::ready()
{
    return data_ready > 1 ? -1 : data_ready;
}

bool BufferDMA::first_half_ready()
{
    return first_half;
}

void BufferDMA::read_half()
{
    data_ready--;
}