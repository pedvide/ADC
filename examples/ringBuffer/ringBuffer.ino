#include "ADC.h"

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A9;

#define ADC_0 0
#define ADC_1 1

ADC *adc = new ADC(); // adc object

RingBufferDMA *buffer = new RingBufferDMA();


void setup() {

    GPIOC_PTOR = 1<<5;

    pinMode(ledPin, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended

    Serial.begin(9600);



    //adc->setAveraging(8); // set number of averages
    //adc->setResolution(12); // set bits of resolution

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

    delay(1000);
    Serial.println("End setup");

    GPIOC_PTOR = 1<<5;
}

int value = ADC_ERROR_VALUE;
char c=0;

void loop() {

    value = adc->analogRead(readPin);

    Serial.print("Writing value: ");
    Serial.println(value*3.3/adc->getMaxValue(ADC_0));

    //buffer->write(value); in isr now

    //Serial.print("Buffer read:");
    //Serial.println(buffer->read()*3.3/adc->getMaxValue(ADC_0));


    if (Serial.available()) {
      c = Serial.read();
      if(c=='d') { // stop DMA
          Serial.println("Stopping DMA.");
          DMA_CERQ = DMA_CERQ_CERQ(3);
          adc->disableDMA(ADC_0);

      } else if(c=='l') { // toggle led
          GPIOC_PTOR = 1<<5;
      } else if(c=='a') { // read DMA_TCD3_DADDR
          Serial.print("DMA_TCD3_DADDR: ");
          Serial.println((int)DMA_TCD3_DADDR, HEX);
      }
  }

    delay(100);

    GPIOC_PTOR = 1<<5;
}

void __attribute__((weak)) dma_ch3_isr(void) {
  //int t = micros();
  //digitalWriteFast(ledPin, !digitalReadFast(ledPin));
  //Serial.print("DMA_CH3_ISR: "); Serial.println(t);
  //GPIOC_PTOR = 1<<5;
    DMA_CINT = 3;
    //digitalWriteFast(ledPin, LOW);

    //buffer->write();


}
