#include "RingBuffer.h"

/* I CAN'T GET NEW[] AND DELETE[] TO WORK....
RingBuffer::RingBuffer()
{
    size = DEFAULT_BUFFER_SIZE;
    elems = new int[size];
}

RingBuffer::RingBuffer(int buffer_size) {
    size = buffer_size;
    elems = new int[size];
}

RingBuffer::~RingBuffer()
{
    delete[] elems;
}
*/

RingBuffer::RingBuffer()
{
    //ctor
}

RingBuffer::~RingBuffer()
{
    // dtor
}

int RingBuffer::isFull() {
    return (end == (start ^ size));
}

int RingBuffer::isEmpty() {
    return (end == start);
}

void RingBuffer::write(int value) {
    elems[end&(size-1)] = value;
    if (isFull()) { /* full, overwrite moves start pointer */
        start = increase(start);
    }
    end = increase(end);
}

int RingBuffer::read() {
    int result = elems[start&(size-1)];
    start = increase(start);
    return result;
}

// increases the pointer modulo 2*size-1
int RingBuffer::increase(int p) {
    return (p + 1)&(2*size-1);
}
