/* Teensy 3.x, LC ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2017 Pedro Villanueva
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

// Constructor
RingBufferDMA::RingBufferDMA(volatile int16_t* elems, uint32_t len, uint8_t ADC_num) :
        p_elems(elems)
        , b_size(len)
        , ADC_number(ADC_num)
        , ADC_RA(&ADC0_RA + (uint32_t)0x20000*ADC_number)
        {

    b_start = 0;
    b_end = 0;


    dmaChannel = new DMAChannel(); // reserve a DMA channel


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}

void RingBufferDMA::start(void (*call_dma_isr)(void)) {

    // set up a DMA channel to store the ADC data
    // The idea is to have ADC_RA as a source,
    // the buffer as a circular buffer
    // each ADC conversion triggers a DMA transfer (transferCount(b_size)), of size 2 bytes (transferSize(2))

    dmaChannel->source(*ADC_RA);

    dmaChannel->destinationCircular((uint16_t*)p_elems, 2*b_size); // 2*b_size is necessary for some reason

    dmaChannel->transferSize(2); // both SRC and DST size

    dmaChannel->transferCount(b_size); // transfer b_size values

    dmaChannel->interruptAtCompletion(); //interruptAtHalf or interruptAtCompletion


	uint8_t DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC0;
	#if ADC_NUM_ADCS>=2
    if(ADC_number==1){
        DMAMUX_SOURCE_ADC = DMAMUX_SOURCE_ADC1;
    }
    #endif // ADC_NUM_ADCS


	dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_ADC); // start DMA channel when ADC finishes a conversion
	dmaChannel->enable();

	dmaChannel->attachInterrupt(call_dma_isr);

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}


RingBufferDMA::~RingBufferDMA() {

    dmaChannel->detachInterrupt();
    dmaChannel->disable();
    delete dmaChannel;
}


bool RingBufferDMA::isFull() {
    //b_end = uint32_t(dmaChannel->destinationAddress()) - uint32_t(p_elems);
    //b_end /= 2; // convert from uint16_t to positions

    return (b_end == (b_start ^ b_size));
}

bool RingBufferDMA::isEmpty() {
    //b_end = uint32_t(dmaChannel->destinationAddress()) - uint32_t(p_elems);
    //b_end /= 2; // convert from uint16_t to positions

    return (b_end == b_start);
}

// update internal pointers
// this gets called only by the isr
void RingBufferDMA::write() {
    // using DMA:
    // call this inside the dma_isr to update the b_start and/or b_end pointers
    if (isFull()) { /* full, overwrite moves start pointer */
        b_start = increase(b_start);
    }
    b_end = increase(b_end);

    dmaChannel->clearInterrupt();

//    b_start = increase(b_start);
//    b_end = uint32_t(dmaChannel->destinationAddress()) - uint32_t(p_elems);
//    b_end /= 2; // convert from uint16_t to positions
}

int16_t RingBufferDMA::read() {

    if(isEmpty()) {
        return 0;
    }

    // using DMA:
    // read last value and update b_start
    int result = p_elems[b_start&(b_size-1)];
    b_start = increase(b_start);
    return result;
}

// increases the pointer modulo 2*size-1
uint16_t RingBufferDMA::increase(uint16_t p) {
    return (p + 1)&(2*b_size-1);
}
