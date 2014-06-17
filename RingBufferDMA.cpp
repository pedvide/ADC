 /* Teensy 3.x ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2014 Pedro Villanueva
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "RingBufferDMA.h"



RingBufferDMA::RingBufferDMA(uint8_t dma_channel, uint8_t ADC_num) {

    b_size = DMA_BUFFER_SIZE;
    b_start = 0;
    b_end = 0;

    if(dma_channel<0) {
        dma_channel = 0;
    }
    if(ADC_num<0) {
        ADC_num = 0;
    } else if(ADC_num>1) {
        ADC_num = 1;
    }

    DMA_channel = dma_channel;

    ADC_number = ADC_num;

    IRQ_DMA_CH = IRQ_DMA_CH0 + DMA_channel;

    // point to the registers corresponding to the DMA_channel
    DMAMUX0_CHCFG = &DMAMUX0_CHCFG0 + DMA_channel;

    uint16_t dma_offset_16 = 0x10;
    uint32_t dma_offset_32 = 0x8;

    DMA_TCD_CSR = &DMA_TCD0_CSR + dma_offset_16*DMA_channel;

    DMA_TCD_SADDR = &DMA_TCD0_SADDR + dma_offset_32*DMA_channel;
    DMA_TCD_SOFF = &DMA_TCD0_SOFF + dma_offset_16*DMA_channel;
    DMA_TCD_SLAST = &DMA_TCD0_SLAST + dma_offset_32*DMA_channel;

    DMA_TCD_DADDR = &DMA_TCD0_DADDR + dma_offset_32*DMA_channel;
    DMA_TCD_DOFF = &DMA_TCD0_DOFF + dma_offset_16*DMA_channel;
    DMA_TCD_DLASTSGA = &DMA_TCD0_DLASTSGA + dma_offset_32*DMA_channel;

    DMA_TCD_ATTR = &DMA_TCD0_ATTR + dma_offset_16*DMA_channel;
    DMA_TCD_NBYTES_MLNO = &DMA_TCD0_NBYTES_MLNO + dma_offset_32*DMA_channel;

    DMA_TCD_CITER_ELINKNO = &DMA_TCD0_CITER_ELINKNO + dma_offset_16*DMA_channel;
    DMA_TCD_BITER_ELINKNO = &DMA_TCD0_BITER_ELINKNO + dma_offset_16*DMA_channel;

    // point to the correct ADC
    ADC_RA = &ADC0_RA + (uint32_t)0x20000*ADC_number;

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));


}

void RingBufferDMA::start() {

    SIM_SCGC7 |= SIM_SCGC7_DMA;
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX;

    DMA_CR = 0; // normal mode of operation

    *DMAMUX0_CHCFG = DMAMUX_DISABLE; // disable before changing

    *DMA_TCD_ATTR = DMA_TCD_ATTR_SSIZE(DMA_TCD_ATTR_SIZE_16BIT) |
                  DMA_TCD_ATTR_DSIZE(DMA_TCD_ATTR_SIZE_16BIT) |
                  DMA_TCD_ATTR_DMOD(4); // src and dst data is 16 bit (2 bytes), buffer size 2^^4 bytes = 8 values
    *DMA_TCD_NBYTES_MLNO = 2; // Minor Byte Transfer Count 2 bytes = 16 bits (we transfer 2 bytes each minor loop)

    *DMA_TCD_SADDR = ADC_RA; // source address
    *DMA_TCD_SOFF = 0; // don't change the address when minor loop finishes
    *DMA_TCD_SLAST = 0; // don't change src address after major loop completes

    *DMA_TCD_DADDR = elems; // destination address
    *DMA_TCD_DOFF = 2; // increment 2 bytes each minor loop
    *DMA_TCD_DLASTSGA = 0; // modulus feature takes care of going back to first element

    *DMA_TCD_CITER_ELINKNO = 1; // Current Major Iteration Count with channel linking disabled (how many minor loops in each major)
    *DMA_TCD_BITER_ELINKNO = 1; // Starting Major Iteration Count with channel linking disabled (how many minor loops in each major)

    *DMA_TCD_CSR = DMA_TCD_CSR_INTMAJOR; // Control and status: interrupt when major counter is complete

    DMA_CERQ = DMA_CERQ_CERQ(DMA_channel); // clear all past request
    DMA_CINT = DMA_channel; // clear interrupts

    uint8_t DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC0;
    if(ADC_number==1){
        DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC1;
    }
    *DMAMUX0_CHCFG = DMAMUX_SOURCE_ADC | DMAMUX_ENABLE; // enable mux and set channel DMA_channel to ADC0

    DMA_SERQ = DMA_SERQ_SERQ(DMA_channel); // enable DMA request
    NVIC_ENABLE_IRQ(IRQ_DMA_CH); // enable interrupts


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}


RingBufferDMA::~RingBufferDMA() {

    DMA_CERQ = DMA_CERQ_CERQ(DMA_channel); // stop channel 3
    NVIC_DISABLE_IRQ(IRQ_DMA_CH);
}



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
    if (isFull()) { /* full, overwrite moves start pointer */
        b_start = increase(b_start);
    }
    b_end = increase(b_end);
}

int RingBufferDMA::read() {
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

