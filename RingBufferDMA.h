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

#ifndef RINGBUFFERDMA_H
#define RINGBUFFERDMA_H

#include <Arduino.h>

// THE SIZE MUST BE A POWER OF 2!!
#define DMA_BUFFER_SIZE 8


/** Class RingBufferDMA implements a circular buffer of fixed size (must be power of 2)
*   Code adapted from http://en.wikipedia.org/wiki/Circular_buffer#Mirroring
*/
class RingBufferDMA
{
    public:
        //! Constructor, buffer has a size DEFAULT_BUFFER_SIZE
        RingBufferDMA(uint8_t dma_channel, uint8_t ADC_num = 0);

        //! Destructor
        ~RingBufferDMA();

        //! Returns 1 (true) if the buffer is full
        bool isFull();

        //! Returns 1 (true) if the buffer is empty
        bool isEmpty();

        //! Write a value into the buffer
        /** The actual value is copied by DMA, this function only updates the buffer pointers to reflect that fact.
        *
        */
        void write();

        //! Read a value from the buffer, make sure it's not emtpy by calling isEmpty() first
        int read();

        //! Start DMA operation
        void start();


        //! Elements of the buffer, aligned to 16 bits
        int16_t elems[DMA_BUFFER_SIZE] __attribute__((aligned(0x10))); // align to 16 bits

        //! DMA channel of the instance
        uint8_t DMA_channel;

        //! ADC module of the instance
        uint8_t ADC_number;


    protected:
    private:

        //! Size of buffer
        uint16_t b_size;

        //! Start pointer: Read here
        uint16_t b_start;
        //! End pointer: Write here
        uint16_t b_end;

        //! Increases the pointer modulo 2*size-1
        uint16_t increase(uint16_t p);


        // Registers to point to the correct DMA channel
        uint8_t IRQ_DMA_CH;

        volatile uint8_t* DMAMUX0_CHCFG;

        volatile uint16_t* DMA_TCD_CSR; // TCD Control and Status

        volatile const void * volatile * DMA_TCD_SADDR; // TCD Source Address
        volatile int16_t* DMA_TCD_SOFF;  // TCD Signed Source Address Offset
        volatile int32_t* DMA_TCD_SLAST; // TCD Last Source Address Adjustment

        volatile void * volatile * DMA_TCD_DADDR; // TCD Destination Address
        volatile int16_t* DMA_TCD_DOFF; // TCD Signed Destination Address Offset
        volatile int32_t* DMA_TCD_DLASTSGA; // TCD Last Destination Address Adjustment/Scatter Gather Address

        volatile uint16_t* DMA_TCD_ATTR; // TCD Transfer Attributes
        volatile uint32_t* DMA_TCD_NBYTES_MLNO; // TCD Minor Byte Count (Minor Loop Disabled)

        volatile uint16_t* DMA_TCD_CITER_ELINKNO; // TCD Current Minor Loop Link, Major Loop Count, Channel Linking Disabled
        volatile uint16_t* DMA_TCD_BITER_ELINKNO; // TCD Beginning Minor Loop Link, Major Loop Count, Channel Linking Disabled

        volatile uint32_t* ADC_RA; // adc module



};


#endif // RINGBUFFERDMA_H
