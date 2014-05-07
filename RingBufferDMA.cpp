#include "RingBufferDMA.h"

// I CAN'T GET NEW[] AND DELETE[] TO WORK....
RingBufferDMA::RingBufferDMA()
{

    GPIOC_PSOR = 1<<5;
    RingBufferDMA(DMA_BUFFER_SIZE);
}

RingBufferDMA::RingBufferDMA(uint32_t buffer_size) {


    //b_size = round_pow2(buffer_size);
    //b_size = 8;
    //elems = new int[8];

    b_size = buffer_size;

    // Point to correct ADC result register
    //ADC_RA = &ADC0_RA + adc_num*(uint32_t)0x20000;;


    if( !(SIM_SCGC7 & SIM_SCGC7_DMA) ) { // start DMA clocks if not already on
        SIM_SCGC7 |= SIM_SCGC7_DMA;
        SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
    }

    DMA_CR = 0; // normal mode of operation

    DMA_TCD3_ATTR = DMA_TCD_ATTR_SSIZE(DMA_TCD_ATTR_SIZE_16BIT) |
                  DMA_TCD_ATTR_DSIZE(DMA_TCD_ATTR_SIZE_16BIT) |
                  DMA_TCD_ATTR_DMOD(4); // src and dst data is 16 bit (2 bytes), buffer size 2^^4 bytes = 8 values
    DMA_TCD3_NBYTES_MLNO = 2; // Minor Byte Transfer Count 2 bytes = 16 bits (we transfer 2 bytes each minor loop)

    DMA_TCD3_SADDR = &ADC0_RA; // source address
    DMA_TCD3_SOFF = 0; // don't change the address when minor loop finishes
    DMA_TCD3_SLAST = 0; // don't change src address after major loop completes

    DMA_TCD3_DADDR = elems; // destination address
    DMA_TCD3_DOFF = 2; // increment 2 bytes each minor loop
    DMA_TCD3_DLASTSGA = 0; // modulus feature takes care of going back to first element

    DMA_TCD3_CITER_ELINKNO = 1; // Current Major Iteration Count with channel linking disabled (how many minor loops in each major)
    DMA_TCD3_BITER_ELINKNO = 1; // Starting Major Iteration Count with channel linking disabled (how many minor loops in each major)

    DMA_TCD3_CSR = DMA_TCD_CSR_INTMAJOR; // Control and status: interrupt when major counter is complete

    DMAMUX0_CHCFG3 = DMAMUX_DISABLE; // disable before changing
    DMAMUX0_CHCFG3 = DMAMUX_SOURCE_ADC0 | DMAMUX_ENABLE; // enable mux and set DMA channel 3 to ADC0

    DMA_SERQ = DMA_SERQ_SERQ(3); // enable DMA request for channel 3
    NVIC_ENABLE_IRQ(IRQ_DMA_CH3);
}

RingBufferDMA::~RingBufferDMA()
{
    //delete[] elems;

    DMA_CERQ = DMA_CERQ_CERQ(3); // stop channel 3
}

/*
RingBufferDMA::RingBufferDMA()
{
    //ctor
}

RingBufferDMA::~RingBufferDMA()
{
    // dtor
}*/

bool RingBufferDMA::isFull() {
    return (b_end == (b_start ^ b_size));
}

bool RingBufferDMA::isEmpty() {
    return (b_end == b_start);
}

// update internal pointers, call from dm_chX_ist
void RingBufferDMA::write() {
    // using DMA:
    // call this inside the dma_isr to update the b_start and/or b_end pointers
    //elems[b_end&(b_size-1)] = value;
    if (isFull()) { /* full, overwrite moves start pointer */
        b_start = increase(b_start);
    }
    b_end = increase(b_end);
}

uint16_t RingBufferDMA::read() {
    // using DMA:
    // read last value and update b_start
    int result = elems[b_start&(b_size-1)];
    b_start = increase(b_start);
    return result;
}

// increases the pointer modulo 2*size-1
uint16_t RingBufferDMA::increase(uint16_t p) {
    return (p + 1)&(2*b_size-1);
}

// Round n up to closest power of 2
uint32_t RingBufferDMA::round_pow2(uint32_t n) {

    n--;
    n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
    n |= n >> 2;   // and then or the results.
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    // The result is a number of 1 bits equal to the number
    // of bits in the original number, plus 1. That's the
    // next highest power of 2.
    return n;
}

void __attribute__((weak)) dma_ch3_isr(void) {
  //int t = micros();
  //digitalWriteFast(ledPin, !digitalReadFast(ledPin));
  //Serial.print("DMA_CH3_ISR: "); Serial.println(t);
  //GPIOC_PTOR = 1<<5;
    DMA_CINT = 3;
    //digitalWriteFast(ledPin, LOW);


}
