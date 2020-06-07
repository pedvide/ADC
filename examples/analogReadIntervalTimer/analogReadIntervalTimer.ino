/*
    This example shows how to use the IntervalTimer library and the ADC library in the Teensy 3.0/3.1

    The three important objects are: the ADC, the (one or more) IntervalTimer and the same number of RingBuffer.

    The ADC sets up the internal ADC, you can change the settings as usual. Enable interrupts.
    The IntervalTimer (timerX) executes a function every 'period' microseconds.
    This function, timerX_callback, starts the desired ADC measurement, for example:
        startSingleRead, startSingleDifferential, startSynchronizedSingleRead, startSynchronizedSingleDifferential
    you can use the ADC0 or ADC1 (only for Teensy 3.1).

    When the measurement is done, the adc_isr is executed:
        - If you have more than one timer per ADC module you need to know which pin was measured.
        - Then you store/process the data
        - Finally, if you have more than one timer you can check whether the last measurement interruped a
          a previous one (using adc->adcX->adcWasInUse), and restart it if so.
          The settings of the interrupted measurement are stored in the adc->adcX->adc_config struct.


*/



#include <ADC.h>
// and IntervalTimer
#include <IntervalTimer.h>

const int ledPin = LED_BUILTIN;

const int readPin0 = A10;
const int period0 = 120; // us

const int readPin1 = A11;
const int period1 = 120; // us

const int readPeriod = 100000; // us

ADC *adc = new ADC(); // adc object

IntervalTimer timer0, timer1; // timers

#define BUFFER_SIZE 500

uint16_t buffer_0[BUFFER_SIZE];
uint16_t buffer_0_count = 0xffff;
uint32_t delta_time_0 = 0;
uint16_t buffer_1[BUFFER_SIZE];
uint16_t buffer_1_count = 0xffff;
uint32_t delta_time_1 = 0;

elapsedMillis timed_read_elapsed;

int startTimerValue0 = 0, startTimerValue1 = 0;

void setup() {

    pinMode(ledPin, OUTPUT); // led blinks every loop

    pinMode(ledPin+1, OUTPUT); // timer0 starts a measurement
    pinMode(ledPin+2, OUTPUT); // timer1 starts a measurement
    pinMode(ledPin+3, OUTPUT); // adc0_isr, measurement finished for readPin0
    pinMode(ledPin+4, OUTPUT); // adc0_isr, measurement finished for readPin1

    pinMode(readPin0, INPUT);
    pinMode(readPin1, INPUT);

    Serial.begin(9600);

    delay(1000);

    ///// ADC0 ////
    adc->adc0->setAveraging(16); // set number of averages
    adc->adc0->setResolution(12); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    Serial.println("Starting Timers");

    // start the timers, if it's not possible, startTimerValuex will be false
    startTimerValue0 = timer0.begin(timer0_callback, period0);
    // wait enough time for the first timer conversion to finish (depends on resolution and averaging),
    // with 16 averages, 12 bits, and ADC_MED_SPEED in both sampling and conversion speeds it takes about 36 us.
    delayMicroseconds(25); // if we wait less than 36us the timer1 will interrupt the conversion
    // initiated by timer0. The adc_isr will restart the timer0's measurement.

    // You can check with an oscilloscope:
    // Pin 14 corresponds to the timer0 initiating a measurement
    // Pin 15 the same for the timer1
    // Pin 16 is the adc_isr when there's a new measurement on readpin0
    // Pin 17 is the adc_isr when there's a new measurement on readpin1

    // Timer0 starts a comversion and 25 us later timer1 starts a new one, "pausing" the first, about 36 us later timer1's conversion
    // is done, and timer0's is restarted, 36 us later timer0's conversion finishes. About 14 us later timer0 starts a new conversion again.
    // (times don't add up to 120 us because the timer_callbacks and adc_isr take time to execute, about 2.5 us and 1 us, respectively)
    // so in the worst case timer0 gets a new value in about twice as long as it would take alone.
    // if you change the periods, make sure you don't go into a loop, with the timers always interrupting each other
    startTimerValue1 = timer1.begin(timer1_callback, period1);

    adc->adc0->enableInterrupts(adc0_isr);

    Serial.println("Timers started");

    delay(500);
}

int value = 0;
char c=0;

void loop() {

    if(startTimerValue0==false) {
            Serial.println("Timer0 setup failed");
    }
    if(startTimerValue1==false) {
            Serial.println("Timer1 setup failed");
    }

    // See if we have a timed read test that finished.
    if (delta_time_0) printTimedADCInfo(ADC_0, buffer_0, delta_time_0);
    if (delta_time_1) printTimedADCInfo(ADC_0, buffer_1, delta_time_1);

    if (Serial.available()) {
        c = Serial.read();
        if(c=='s') { // stop timer
            Serial.println("Stop timer1");
            timer1.end();
        } else if(c=='r') { // restart timer
            Serial.println("Restart timer1");
            startTimerValue1 = timer1.begin(timer1_callback, period1);
        } else if(c=='p') { // restart timer
            Serial.print("isContinuous: ");
            Serial.println(adc->adc0->isContinuous());
        }
    }

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );

    delayMicroseconds(readPeriod);
}

void printTimedADCInfo(uint8_t adc_num, uint16_t *buffer, uint32_t &delta_time) {
  uint32_t min_value = 0xffff;
  uint32_t max_value = 0;
  uint32_t sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (buffer[i] < min_value) min_value = buffer[i];
    if (buffer[i] > max_value) max_value = buffer[i];
    sum += buffer[i];
  }
  float average_value = (float)sum / BUFFER_SIZE; // get an average...
  float sum_delta_sq = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    int delta_from_center = (int)buffer[i] - average_value;
    sum_delta_sq += delta_from_center * delta_from_center;
  }
  int rms = sqrt(sum_delta_sq / BUFFER_SIZE);
  Serial.printf("ADC:%d delta time:%d freq:%d - min:%d max:%d avg:%d rms:%d\n", adc_num,
                delta_time, (1000 * BUFFER_SIZE) / delta_time,
                min_value, max_value, (int)average_value, rms);

  delta_time = 0;

}

// This function will be called with the desired frequency
// start the measurement
// in my low-res oscilloscope this function seems to take 1.5-2 us.
void timer0_callback(void) {

    digitalWriteFast(ledPin+1, HIGH);

    adc->adc0->startSingleRead(readPin0); // also: startSingleDifferential, analogSynchronizedRead, analogSynchronizedReadDifferential

    digitalWriteFast(ledPin+1, LOW);
    //digitalWriteFast(ledPin+1, !digitalReadFast(ledPin+1));

}

// This function will be called with the desired frequency
// start the measurement
void timer1_callback(void) {

    digitalWriteFast(ledPin+2, HIGH);

    adc->adc0->startSingleRead(readPin1);

    digitalWriteFast(ledPin+2, LOW);

}

// when the measurement finishes, this will be called
// first: see which pin finished and then save the measurement into the correct buffer
void adc0_isr() {


#if defined(__IMXRT1062__)  // Teensy 4.0
    uint8_t pin = ADC::sc1a2channelADC0[ADC1_HC0&0x1f]; // the bits 0-4 of ADC0_SC1A have the channel
#else
    uint8_t pin = ADC::sc1a2channelADC0[ADC0_SC1A&ADC_SC1A_CHANNELS]; // the bits 0-4 of ADC0_SC1A have the channel
#endif
    // add value to correct buffer
    if(pin==readPin0) {
        digitalWriteFast(ledPin+3, HIGH);
        uint16_t adc_val = adc->adc0->readSingle();
        if (buffer_0_count < BUFFER_SIZE) {
          buffer_0[buffer_0_count++] = adc_val;
          if (buffer_0_count == BUFFER_SIZE) delta_time_0 = timed_read_elapsed;
        }
        digitalWriteFast(ledPin+3, LOW);
        
    } else if(pin==readPin1) {
        digitalWriteFast(ledPin+4, HIGH);
        uint16_t adc_val = adc->adc0->readSingle();
        if (buffer_1_count < BUFFER_SIZE) {
          buffer_1[buffer_1_count++] = adc_val;
          if (buffer_1_count == BUFFER_SIZE) delta_time_1 = timed_read_elapsed;
        }
        digitalWriteFast(ledPin+4, LOW);
    } else { // clear interrupt anyway
        adc->readSingle();
    }

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc->adc0->adcWasInUse) {
        // restore ADC config, and restart conversion
        adc->adc0->loadConfig(&adc->adc0->adc_config);
        // avoid a conversion started by this isr to repeat itself
        adc->adc0->adcWasInUse = false;
    }


    //digitalWriteFast(ledPin+2, !digitalReadFast(ledPin+2));

    #if defined(__IMXRT1062__)  // Teensy 4.0
        asm("DSB");
    #endif

}