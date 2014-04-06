#include "RingBuffer.h"

// I CAN'T GET NEW[] AND DELETE[] TO WORK....
/*RingBuffer::RingBuffer()
{
    RingBuffer(DEFAULT_BUFFER_SIZE);
}

RingBuffer::RingBuffer(int buffer_size) {
    if(buffer_size%2) { // not a power of 2
        size = (int)ceil(1.443*log(buffer_size));
    }
    b_size = buffer_size;
    elems = new int[1024];
}

RingBuffer::~RingBuffer()
{
    delete[] elems;
}*/


RingBuffer::RingBuffer()
{
    //ctor
}

RingBuffer::~RingBuffer()
{
    // dtor
}

int RingBuffer::isFull() {
    return (b_end == (b_start ^ b_size));
}

int RingBuffer::isEmpty() {
    return (b_end == b_start);
}

void RingBuffer::write(int value) {
    elems[b_end&(b_size-1)] = value;
    if (isFull()) { /* full, overwrite moves start pointer */
        b_start = increase(b_start);
    }
    b_end = increase(b_end);
}

int RingBuffer::read() {
    int result = elems[b_start&(b_size-1)];
    b_start = increase(b_start);
    return result;
}

// increases the pointer modulo 2*size-1
int RingBuffer::increase(int p) {
    return (p + 1)&(2*b_size-1);
}
