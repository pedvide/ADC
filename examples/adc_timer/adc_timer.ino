/* Example for triggering the ADC with Timer
    Valid for Teensy 3.0 and 3.1
*/


#include <ADC.h>
#include <ADC_util.h>

const int readPin = A0; // ADC0
#define USE_ADC_0
#define USE_ADC_1

#if ADC_NUM_ADCS>1
const int readPin2 = A1; // ADC1
#endif

ADC *adc = new ADC(); // adc object;

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
  adc->setAveraging(1); // set number of averages
  adc->setResolution(8); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed

  ////// ADC1 /////
#if ADC_NUM_ADCS>1
  pinMode(readPin2, INPUT);
  adc->setAveraging(1, ADC_1); // set number of averages
  adc->setResolution(8, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_1); // change the sampling speed
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
      value = (uint16_t)adc->readSingle(ADC_0); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
      Serial.printf("%d = ", value);
      Serial.println(value * 3.3 / adc->getMaxValue(ADC_0), DEC);
#if ADC_NUM_ADCS>1 && defined(USE_ADC_1)
      Serial.print("Value ADC1: ");
      value2 = (uint16_t)adc->readSingle(ADC_1); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
      Serial.printf("%d = ", value2);
      Serial.println(value2 * 3.3 / adc->getMaxValue(ADC_1), DEC);
#endif
    } else if (c == 's') { // start Timer, before pressing enter write the frequency in Hz
      uint32_t freq = Serial.parseInt();
      if (freq == 0) {
        Serial.println("Stop Timer.");
        adc->adc0->stopTimer();
        adc->adc1->stopTimer();
      }
      else {
        Serial.print("Start Timer with frequency ");
        Serial.print(freq);
        Serial.println(" Hz.");
        adc->adc0->stopTimer();
        adc->adc0->startSingleRead(readPin); // call this to setup everything before the Timer starts, differential is also possible
        adc->adc0->enableInterrupts(adc0_isr);
        adc->adc0->startTimer(freq); //frequency in Hz
#if ADC_NUM_ADCS>1 && defined(USE_ADC_1)
        adc->adc1->stopTimer();
        adc->adc1->startSingleRead(readPin2); // call this to setup everything before the Timer starts
        adc->adc1->enableInterrupts(adc1_isr);
        adc->adc1->startTimer(freq); //frequency in Hz
#endif
        Serial.println("\n*** ADC and ADC_ETC ***");
        Serial.printf("ADC1: HC0:%x HS:%x CFG:%x GC:%x GS:%x\n", IMXRT_ADC1.HC0, IMXRT_ADC1.HS,  IMXRT_ADC1.CFG, IMXRT_ADC1.GC, IMXRT_ADC1.GS);
        Serial.printf("ADC2: HC0:%x HS:%x CFG:%x GC:%x GS:%x\n", IMXRT_ADC2.HC0, IMXRT_ADC2.HS,  IMXRT_ADC2.CFG, IMXRT_ADC2.GC, IMXRT_ADC2.GS);
        Serial.printf("ADC_ETC: CTRL:%x DONE0_1:%x DONE2_ERR:%x DMA: %x\n", IMXRT_ADC_ETC.CTRL,
                      IMXRT_ADC_ETC.DONE0_1_IRQ, IMXRT_ADC_ETC.DONE2_ERR_IRQ, IMXRT_ADC_ETC.DMA_CTRL);
        for (uint8_t trig = 0; trig < 8; trig++) {
          Serial.printf("    TRIG[%d] CTRL: %x CHAIN_1_0:%x\n",
                        trig, IMXRT_ADC_ETC.TRIG[trig].CTRL, IMXRT_ADC_ETC.TRIG[trig].CHAIN_1_0);
        }

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
#if ADC_NUM_ADCS>1 && defined(USE_ADC_1)
  if (adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
    Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
  }
#endif
  adc->resetError();

  // See if we have a timed read test that finished.
  if (delta_time_adc_0)printTimedADCInfo(ADC_0, buffer_ADC_0, delta_time_adc_0);
  if (delta_time_adc_1)printTimedADCInfo(ADC_1, buffer_ADC_1, delta_time_adc_1);

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

#if ADC_NUM_ADCS>1
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
