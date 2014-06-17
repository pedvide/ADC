#include "ADC.h"

const int readPin = A9;

ADC *adc = new ADC(); // adc object

RingBufferDMA *dmaBuffer = new RingBufferDMA(2,ADC_0); // use dma channel 2 and ADC0


void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended

    Serial.begin(9600);

    //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

    adc->setAveraging(8); // set number of averages
    adc->setResolution(12); // set bits of resolution


    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

    // enable DMA and interrupts
    adc->enableDMA(ADC_0);
    adc->enableInterrupts(ADC_0);

}

char c=0;


void loop() {

     if (Serial.available()) {
      c = Serial.read();
      if(c=='s') { // start dma
            Serial.println("Start DMA");
            dmaBuffer->start();
      } else if(c=='c') { // start conversion
          Serial.println("Conversion: ");
          adc->analogRead(readPin, ADC_0);
      } else if(c=='p') { // print buffer
          printBuffer();
      } else if(c=='l') { // toggle led
          digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
      } else if(c=='r') { // read
          Serial.print("read(): ");
          Serial.println(dmaBuffer->read());
      } else if(c=='f') { // full?
          Serial.print("isFull(): ");
          Serial.println(dmaBuffer->isFull());
      } else if(c=='e') { // empty?
          Serial.print("isEmpty(): ");
          Serial.println(dmaBuffer->isEmpty());
      }
  }


    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    delay(100);
}

// this function is called everytime a new value is converted by the ADC and copied by DMA
void dma_ch2_isr(void) {
    //int t = micros();
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    Serial.println("DMA_CH2_ISR"); //Serial.println(t);

    DMA_CINT = 2; // clear interrupt

    // call write to tell the buffer that we wrote a new value
    dmaBuffer->write();

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

}
void dma_ch10_isr(void) {
    //int t = micros();
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    Serial.println("DMA_CH10_ISR"); //Serial.println(t);

    DMA_CINT = 10; // clear interrupt

    // call write to tell the buffer that we wrote a new value
    dmaBuffer->write();

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

}

// called everytime a new value is converted. The DMA isr is called first
void adc0_isr(void) {
    //int t = micros();
    Serial.println("ADC0_ISR"); //Serial.println(t);
    ADC0_RA; // clear interrupt
}
void adc1_isr(void) {
    //int t = micros();
    Serial.println("ADC1_ISR"); //Serial.println(t);
    ADC1_RA; // clear interrupt
}


void printBuffer() {
  Serial.println("Buffer: Address, Value");
  uint8_t i = 0;
  for (i = 0; i < DMA_BUFFER_SIZE; i++) {
    Serial.print(int32_t(&dmaBuffer->elems[i]), HEX);
    Serial.print(", ");
    Serial.println(dmaBuffer->elems[i]);
  }
}

