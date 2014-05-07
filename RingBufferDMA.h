#ifndef RINGBUFFERDMA_H
#define RINGBUFFERDMA_H

// include new and delete
#include <Arduino.h>

// THE SIZE MUST BE A POWER OF 2!!
#define DMA_BUFFER_SIZE 8


/** Class RingBufferDMA implements a circular buffer of fixed size (must be power of 2)
*   Code adapted from http://en.wikipedia.org/wiki/Circular_buffer#Mirroring
*/
class RingBufferDMA
{
    public:
        //! Default constructor, buffer has a size DEFAULT_BUFFER_SIZE
        RingBufferDMA();

        //! Buffer has a size buffer_size
        /**
        * \param buffer_size must be a power of two!
        */
        RingBufferDMA(uint32_t buffer_size);

        /** Default destructor */
        virtual ~RingBufferDMA();

        //! Returns 1 (true) if the buffer is full
        bool isFull();

        //! Returns 1 (true) if the buffer is empty
        bool isEmpty();

        //! Write a value into the buffer
        void write();

        //! Read a value from the buffer
        uint16_t read();

    protected:
    private:

        uint32_t round_pow2(uint32_t n);

        uint16_t increase(uint16_t p);

        uint16_t b_size = DMA_BUFFER_SIZE;
        uint16_t b_start = 0;
        uint16_t b_end = 0;
        //int *elems;
        //DMAMEM static uint16_t elems[DMA_BUFFER_SIZE];
        uint16_t elems[DMA_BUFFER_SIZE]; // __attribute__((aligned(0x10))); // align to 16 bits

        // registers point to the correct ADC module
        //typedef volatile uint32_t* reg;
        //reg ADC_RA;

};


#endif // RINGBUFFERDMA_H
