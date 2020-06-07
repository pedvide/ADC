/* Example for triggering the ADC with PDB
*   Valid for Teensy 3.0 and 3.1
*/


#include <ADC.h>
#include <ADC_util.h>

const int readPin = A9; // ADC0
const int readPin2 = A2; // ADC1

ADC* adc = new ADC(); // adc object;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(9600);

    Serial.println("Begin setup");

    ///// ADC0 ////
    adc->adc0->setAveraging(1); // set number of averages
    adc->adc0->setResolution(8); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed

    ////// ADC1 /////
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(1); // set number of averages
    adc->adc1->setResolution(8); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
    #endif

    Serial.println("End setup");

}

char c=0;
int value;
int value2;

void loop() {
    #ifdef ADC_USE_PDB

    if (Serial.available()) {
        c = Serial.read();
        if(c=='v') { // value
            Serial.print("Value ADC0: ");
            value = (uint16_t)adc->adc0->readSingle(); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value*3.3/adc->adc0->getMaxValue(), DEC);
            #ifdef ADC_DUAL_ADCS
            Serial.print("Value ADC1: ");
            value2 = (uint16_t)adc->adc1->readSingle(); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value2*3.3/adc->adc1->getMaxValue(), DEC);
            #endif
        } else if(c=='s') { // start pdb, before pressing enter write the frequency in Hz
            uint32_t freq = Serial.parseInt();
            if (freq == 0) {
                Serial.println("Stop pdb.");
                adc->adc0->stopPDB();
                #ifdef ADC_DUAL_ADCS
                adc->adc1->stopPDB();
                #endif
            }
            else {
                Serial.print("Start pdb with frequency ");
                Serial.print(freq);
                Serial.println(" Hz.");
                adc->adc0->stopPDB();
                adc->adc0->startSingleRead(readPin); // call this to setup everything before the pdb starts, differential is also possible
                adc->adc0->enableInterrupts(adc0_isr);
                adc->adc0->startPDB(freq); //frequency in Hz
                #ifdef ADC_DUAL_ADCS
                adc->adc1->stopPDB();
                adc->adc1->startSingleRead(readPin2); // call this to setup everything before the pdb starts
                adc->adc1->enableInterrupts(adc1_isr);
                adc->adc1->startPDB(freq); //frequency in Hz
                #endif
            }
        } else if(c=='p') { // pbd stats
            Serial.print("Frequency: ");
            Serial.println(adc->adc0->getPDBFrequency());
        }

    }


    // Print errors, if any.
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
    }
    #ifdef ADC_DUAL_ADCS
    if(adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
    }
    #endif
    adc->resetError();

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(10);

    #endif // ADC_USE_PDB
}


// Make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
}

#ifdef ADC_DUAL_ADCS
void adc1_isr() {
        adc->adc1->readSingle();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
}
#endif

#ifdef ADC_USE_PDB
// pdb interrupt is enabled in case you need it.
void pdb_isr(void) {
        PDB0_SC &=~PDB_SC_PDBIF; // clear interrupt
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
}
#endif
