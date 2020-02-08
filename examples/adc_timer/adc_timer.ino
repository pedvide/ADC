/* Example for triggering the ADC with Timer
    Valid for the current Teensy 3.x and 4.0.

    On Teensy 3.x this uses the PDB timer, and should work the same as the example adc_pdb.ino, but
    has been extended some to add additional command to read in a whole buffer of readings, using the
    timer. 

    On Teensy 4, this uses one or two of the unused QTimers. 

    Setting it up:  The variables readPin must be defined for a pin that is valid for the first (or only)
    ADC.  If the processor has a second ADC and is enabled, than readPin2 must be configured to be a pin
    that is valid on the second ADC.

    Example usage: 
        Start the timers at some frequency: s 3000<cr>
        get a single read from the ADC(s): v<cr>
        print out the actual frequency: p<cr>
        Read in a whole buffer and print out data: t<cr>
        Stop the timers: s<cr>
*/

#ifdef ADC_USE_TIMER

#include <ADC.h>
#include <ADC_util.h>

const int readPin = A0; // ADC0
#ifdef ADC_DUAL_ADCS
const int readPin2 = A2; // ADC1
#endif

ADC *adc = new ADC(); // adc object;

#define USE_ADC_0
#define USE_ADC_1

#define BUFFER_SIZE 500

uint16_t buffer_ADC_0[BUFFER_SIZE];
uint16_t buffer_adc_0_count = 0xffff;
uint32_t delta_time_adc_0 = 0;
uint16_t buffer_ADC_1[BUFFER_SIZE];
uint16_t buffer_adc_1_count = 0xffff;
uint32_t delta_time_adc_1 = 0;

elapsedMillis timed_read_elapsed;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readPin, INPUT);

  Serial.begin(9600);
  while (!Serial && millis() < 5000) ; // wait up to 5 seconds for serial monitor.

  Serial.println("Begin setup");

  ///// ADC0 ////
  adc->adc0->setAveraging(1); // set number of averages
  adc->adc0->setResolution(8); // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed

  ////// ADC1 /////
#ifdef ADC_DUAL_ADCS
  pinMode(readPin2, INPUT);
  adc->adc1->setAveraging(1); // set number of averages
  adc->adc1->setResolution(8); // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
#endif

  Serial.println("End setup");

  Serial.println("Enter a command such as: s 3000<cr> to start doing something");

}

char c = 0;
int value;
int value2;

void loop() {

  if (Serial.available()) {
    c = Serial.read();
    if (c == 'v') { // value
      Serial.print("Value ADC0: ");
      value = (uint16_t)adc->adc0->readSingle(); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
      Serial.printf("%d = ", value);
      Serial.println(value * 3.3 / adc->adc0->getMaxValue(), DEC);
      #if defined(ADC_DUAL_ADCS) && defined(USE_ADC_1)
      Serial.print("Value ADC1: ");
      value2 = (uint16_t)adc->adc1->readSingle(); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
      Serial.printf("%d = ", value2);
      Serial.println(value2 * 3.3 / adc->adc1->getMaxValue(), DEC);
      #endif
    } else if (c == 's') { // start Timer, before pressing enter write the frequency in Hz
      uint32_t freq = Serial.parseInt();
      if (freq == 0) {
        Serial.println("Stop Timer.");
        adc->adc0->stopTimer();
        #ifdef ADC_DUAL_ADCS
        adc->adc1->stopTimer();
        #endif
      }
      else {
        Serial.print("Start Timer with frequency ");
        Serial.print(freq);
        Serial.println(" Hz.");
        adc->adc0->stopTimer();
        adc->adc0->startSingleRead(readPin); // call this to setup everything before the Timer starts, differential is also possible
        adc->adc0->enableInterrupts(adc0_isr);
        adc->adc0->startTimer(freq); //frequency in Hz
        #if defined(ADC_DUAL_ADCS) && defined(USE_ADC_1)
        adc->adc1->stopTimer();
        adc->adc1->startSingleRead(readPin2); // call this to setup everything before the Timer starts
        adc->adc1->enableInterrupts(adc1_isr);
        adc->adc1->startTimer(freq); //frequency in Hz
        #endif
      }
    } else if (c == 'p') { // print Timer stats
      Serial.print("Frequency: ");
      Serial.println(adc->adc0->getTimerFrequency());
    } else if (c == 't') { // Lets try a timed read
      timed_read_elapsed = 0;
      buffer_adc_0_count = 0;
      buffer_adc_1_count = 0;
      Serial.println("Starting Timed read");
    }
  }


  // Print errors, if any.
  if (adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
    Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
  }
#if defined(ADC_DUAL_ADCS) && defined(USE_ADC_1)
  if (adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
    Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
  }
#endif
  adc->resetError();

  // See if we have a timed read test that finished.
  if (delta_time_adc_0) printTimedADCInfo(ADC_0, buffer_ADC_0, delta_time_adc_0);
  if (delta_time_adc_1) printTimedADCInfo(ADC_1, buffer_ADC_1, delta_time_adc_1);

  //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  delay(10);
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


// Make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
  uint16_t adc_val = adc->adc0->readSingle();
  if (buffer_adc_0_count < BUFFER_SIZE) {
    buffer_ADC_0[buffer_adc_0_count++] = adc_val;
    if (buffer_adc_0_count == BUFFER_SIZE) delta_time_adc_0 = timed_read_elapsed;
  }
  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
#if defined(__IMXRT1062__)  // Teensy 4.0
  asm("DSB");
#endif
}

#ifdef ADC_DUAL_ADCS
void adc1_isr() {
  uint16_t adc_val = adc->adc1->readSingle();
  if (buffer_adc_1_count < BUFFER_SIZE) {
    buffer_ADC_1[buffer_adc_1_count++] = adc_val;
    if (buffer_adc_1_count == BUFFER_SIZE) delta_time_adc_1 = timed_read_elapsed;
  }
#if defined(__IMXRT1062__)  // Teensy 4.0
  asm("DSB");
#endif
  //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
}
#endif

#else // make sure the example can run for any boards (automated testing)
void setup() {}
void loop() {}
#endif // ADC_USE_TIMER