#ifndef RINGBUFFER_H
#define RINGBUFFER_H

// include new and delete
#include <Arduino.h>

// THE SIZE MUST BE A POWER OF 2!!
#define DEFAULT_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

/** Class RingBuffer implements a circular buffer of fixed size (must be power of 2)
*   Code adapted from http://en.wikipedia.org/wiki/Circular_buffer#Mirroring
*/
class RingBuffer
{
    public:
        //! Default constructor, buffer has a size DEFAULT_BUFFER_SIZE
        RingBuffer();

        //! Buffer has a size buffer_size
        /**
        * \param buffer_size must be a power of two!
        */
        //RingBuffer(int buffer_size);

        /** Default destructor */
        virtual ~RingBuffer();

        //! Returns 1 (true) if the buffer is full
        int isFull();

        //! Returns 1 (true) if the buffer is empty
        int isEmpty();

        //! Write a value into the buffer
        void write(int value);

        //! Read a value from the buffer
        int read();

    protected:
    private:

        int increase(int p);

        int size = DEFAULT_BUFFER_SIZE;
        int start = 0;
        int end = 0;
        //int *elems;
        int elems[DEFAULT_BUFFER_SIZE];
};

#ifdef __cplusplus
}
#endif

#endif // RINGBUFFER_H
