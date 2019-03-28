/**
 * This example how to use DMA to transfor ADC results into a buffer.
 * While one half of the buffer is being filled up by DMA transfers, the
 *  other half can be safely accessed and processed.
*/

#include <Arduino.h>
#include "ADC.h"
#include "BufferDMA.h"

const int readPin = A9;   // the pin to be sampled
const int bufferLen = 16; // length of the buffer

void setup()
{
    pinMode(readPin, INPUT); 
    
    // ADC settings
    ADC *adc = new ADC();
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc->setAveraging(1);
    adc->setResolution(12);
    adc->enableDMA(ADC_0);

    // actual DMA destination buffer, as noted in K66 reference manual 24.4.2
    // the addresses and offsets must be aligned on 0-modulo-transfer-size boundaries,
    // so it is only necessary to do alignas(2), but I choose to do 8 bytes align in this case
    alignas(8) static volatile int16_t buffer[bufferLen];
    // set all bits to 0 so we can see the buffer being written. In practice we don't need to do this.
    memset((void *)buffer, 0x0, sizeof(buffer));

    // do not make multiple instances of BufferDMA, as noted in BufferDMA.h
    BufferDMA dmaBuffer(buffer, bufferLen, ADC_0);

    Serial.begin(115200);
    while (!Serial)
    {
    }
    do
    {
        delay(10);
    } while (Serial.available() && Serial.read());
    Serial.println("Press Any Key to Start");
    while (!Serial.available())
    {
    }

    dmaBuffer.start();

    for (int i = 0; i < bufferLen * 4; i++)
    {
        // use non-blocking startSingleRead()
        // you can trigger ADC sampling else where
        adc->adc0->startSingleRead(readPin);

        // delay a bit so ADC conversion and DMA transfer is completed, not necessary in practice.
        // this makes sure we do not miss out last number when we print the results
        delay(5);

        // print the buffer to serial
        for (int j = 0; j < bufferLen; j++)
        {
            Serial.printf("%6d", buffer[j]);
        }
        Serial.println();

        if (dmaBuffer.ready() == 1)
        {
            Serial.print((dmaBuffer.first_half_ready() ? "First" : "Second"));
            Serial.println(" half is ready");

            // do something with data, just set them to 0 in this case.
            memset((void *)(buffer + (!dmaBuffer.first_half_ready()) * bufferLen / 2), 0x0, sizeof(buffer) / 2);

            // mark the lastest available half as read, comment out this
            // line to see what happens if there is a buffer overrun
            dmaBuffer.read_half();
        }
        else if (dmaBuffer.ready() == -1)
        {
            Serial.println("Buffer Overrun!");
            for (;;)
                ;
        }
    }

    Serial.println("Finished");
}

void loop()
{
    // put your main code here, to run repeatedly:
}