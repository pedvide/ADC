// test program to print a histogram of 1 second worth of measurements from each ADC, for all ADC configurations.
// by Eric Fontaine 2021-2022 under The Unlicense (Public Domain).
// newer version may be avilable at https://github.com/ericfont/teensy_histogram_measure_ADC
// only tested on a Teensy 4.1 (may work on other teeny's though may need modifications)
// uses https://github.com/pedvide/ADC library

#include <ADC.h>
#include <ADC_util.h>

ADC *adc = new ADC(); // adc object

// number of ADCs to measure, either just 1 or 2.
#define nADCs 2

const uint32_t adc0_ReadPin = A0;
const uint32_t adc1_ReadPin = A1; // pin 14, 15

const uint32_t nMillisecondsPerConfig = 1000; // number of milliseconds to run measurements for each configuration

void setup() {  
  pinMode(adc0_ReadPin, INPUT_DISABLE);
  pinMode(adc1_ReadPin, INPUT_DISABLE);
  
  Serial.begin(12000000);
}

// space to hold the historgram for each ADC. Each index represents a bin for each possible ADC converstion integer.  Max ADC bits is 12, so reserve space for 2^12 bins.
volatile uint32_t measurementHistogram[nADCs][4096];

uint32_t runNumber = 0;

void loop() {
  
  Serial.println("Running all combinations of resolutions, conversion speeds, and sampling speeds");
  
  for (auto analogReadAveragingNum : averages_list)
  {
    adc->adc0->setAveraging(analogReadAveragingNum); // set number of averages
    adc->adc1->setAveraging(analogReadAveragingNum); // set number of averages
    
    for (auto analogReadBitDepth : resolutions_list)
    {
      adc->adc0->setResolution(analogReadBitDepth); // set bits of resolution
      adc->adc1->setResolution(analogReadBitDepth); // set bits of resolution
      
      uint32_t analogReadBins = (1 << analogReadBitDepth);
      uint32_t analogReadMax = analogReadBins - 1;
      
      for (auto conv_speed : conversion_speed_list)
      {
        adc->adc0->setConversionSpeed(conv_speed); // change the conversion speed
        adc->adc1->setConversionSpeed(conv_speed); // change the conversion speed
        
        for (auto samp_speed : sampling_speed_list)
        {
          adc->adc0->setSamplingSpeed(samp_speed); // change the sampling speed
          adc->adc1->setSamplingSpeed(samp_speed); // change the sampling speed
  
          adc->adc0->wait_for_cal(); // waits until calibration is finished and writes the corresponding registers
          adc->adc1->wait_for_cal(); // waits until calibration is finished and writes the corresponding registers

          runNumber += 1;
              
          // reset histogram counts for both ADCs
          for( uint32_t adc_number = 0; adc_number < nADCs; adc_number++ ) {
            for( uint32_t i = 0; i < 4096; i++ ) {
              measurementHistogram[adc_number][i] = 0;
            }
          }
                
          // take measurements
          uint32_t microsMeasurementStartTime = micros(); 
          
          adc->adc0->enableInterrupts(adc0_isr);
          adc->adc0->startContinuous(adc0_ReadPin);
          
#if (nADCs == 2)
          adc->adc1->enableInterrupts(adc1_isr);
          adc->adc1->startContinuous(adc1_ReadPin);
#endif

          delay(nMillisecondsPerConfig); // take measurements for a while
          
          adc->adc0->stopContinuous();
          adc->adc0->disableInterrupts();
          
#if (nADCs == 2)
          adc->adc1->stopContinuous();
          adc->adc1->disableInterrupts();
#endif
          
          uint32_t microsMeasurementDuration = micros() - microsMeasurementStartTime;
           
          
          // end of taking measurements, now time to print summary statistics
          Serial.print("run #");
          Serial.print(runNumber);
          Serial.print(" starting at ");
          Serial.print(microsMeasurementStartTime / 1000000.0f);
          Serial.print(" seconds for ");
          Serial.print(microsMeasurementDuration);
          Serial.print(" microseconds, producing cumulative histogram.\n");
          Serial.println();
        
          // calculate & print stats
          for( uint32_t adc_number = 0; adc_number < nADCs; adc_number++ ) {
          
            uint32_t minimum_index = (1 << analogReadBitDepth);    
            uint32_t maximum_index = 0;
            uint32_t mode_index = 0;
            uint32_t mode_value = 0;
            float summation = 0;
            uint32_t nMeasurements = 0;
            
            for( uint32_t i = 0; i < analogReadBins; i++) {
              if( measurementHistogram[adc_number][i] > 0 ) {
                nMeasurements += measurementHistogram[adc_number][i];
                summation += (float)(measurementHistogram[adc_number][i] * i) / analogReadMax;
            
                if( i < minimum_index )
                  minimum_index = i;
                  
                if( i > maximum_index )
                  maximum_index = i;
        
                if( measurementHistogram[adc_number][i] > mode_value ) {
                  mode_value = measurementHistogram[adc_number][i];
                  mode_index = i;
                }        
              }
            }
            float mean = (float) summation / nMeasurements;
        
            Serial.print("ADC");
            Serial.print(adc_number);
            Serial.print(" config: ");
            Serial.print("BitDepth=");
            Serial.print(analogReadBitDepth);
            Serial.print(", averaging=");
            Serial.print(analogReadAveragingNum);
            Serial.print(", ADC_CONVERSION_SPEED=");
            Serial.print(getConversionEnumStr(conv_speed));
            Serial.print(", ADC_SAMPLING_SPEED=");
            Serial.print(getSamplingEnumStr(samp_speed));
            Serial.println(".");

            Serial.print(nMeasurements);
            Serial.print(" measurements, at a rate of ");
            float microsPerMeasurement = (float) microsMeasurementDuration / (float) nMeasurements;
            Serial.print(microsPerMeasurement, 3);
            Serial.print(" microseconds/measurement or ");
            Serial.print(1000.0f / microsPerMeasurement);
            Serial.println(" kHz.");
            
            Serial.println("bin    [index]     count    percent of total measurements");
            
            float sumofsquares = 0;
              
            for( uint32_t i = 0; i < analogReadBins; i++) {
              float bin = (float) i / analogReadMax;
              float differenceFromMean = bin - mean;
              sumofsquares += (float) measurementHistogram[adc_number][i] * (differenceFromMean * differenceFromMean);
        
              if( (i + 16 > mode_index) && (i <= mode_index + 16) ) {
                Serial.print(bin, 4);
                Serial.print(" [");
                Serial.print(i);
                Serial.print("]: ");
                printRightJustifiedUnsignedInt(measurementHistogram[adc_number][i]);
                if( measurementHistogram[adc_number][i] > 0 ) {
                  float percentOfTotal = (float) measurementHistogram[adc_number][i] * 100.0f / nMeasurements;
                  Serial.print(' ');
                  for( int32_t bars = (int32_t) measurementHistogram[adc_number][i] * 100 / nMeasurements; bars >= 0; bars-- ) {
                    Serial.write('=');
                  }
                  Serial.print(' ');
                  Serial.print(percentOfTotal);
                  Serial.print('%');
                }
                Serial.println();
              }
            }
                
            Serial.println("Summary:");
            Serial.print("range:  ");
            Serial.print((float) (maximum_index + 1 - minimum_index) / analogReadMax, 9);
            Serial.print(" over ");
            Serial.print(maximum_index - minimum_index + 1);
            Serial.print(" bins from [");
            Serial.print(minimum_index);
            Serial.print("] to [");
            Serial.print(maximum_index);
            Serial.println("]");
            
            Serial.print("mode:   ");
            Serial.print(mode_index / (float) analogReadMax, 9);
            Serial.print(" [");
            Serial.print(mode_index);
            Serial.println("]");
            
            Serial.print("mean:   ");
            Serial.println(mean, 9);
          
            float variance = sumofsquares / (float) nMeasurements;  
            Serial.print("var:    ");
            Serial.println(variance, 9);
          
            float standardDeviation = sqrt(variance);
            Serial.print("stdDev: ");
            Serial.println(standardDeviation, 9);
          
            Serial.println();
          }
        }
      }
    }
  }
  
  Serial.println("Done With All Configurations!  Going to sleep for a long while...now would be a good time to save the contents of this output to a text file, so can analyze later. :)");
  delay(1000000); // wait a long while
}

void adc0_isr(void) {
  uint32_t measurement = adc->adc0->analogReadContinuous();
  measurementHistogram[0][measurement] += 1;
}

#if (nADCs == 2)
void adc1_isr(void) {
  uint32_t measurement = adc->adc1->analogReadContinuous();
  measurementHistogram[1][measurement] += 1;
}
#endif

void printRightJustifiedUnsignedInt(uint32_t value) {
  const int32_t maxDigits = 10;
  uint32_t digits[maxDigits];
  int32_t digitIndex = 0;
  while( digitIndex < maxDigits ) {
    digits[digitIndex] = value % 10;
    value = value / 10;

    if( value == 0 ) {
      for( int32_t digitsLeft = digitIndex + 1; digitsLeft < maxDigits; digitsLeft++ ) {
        Serial.write(' ');
      }
      break;
    }

    digitIndex++;
  }

  while (digitIndex >= 0) {
    Serial.print(digits[digitIndex]);
    digitIndex--;
  }
}
