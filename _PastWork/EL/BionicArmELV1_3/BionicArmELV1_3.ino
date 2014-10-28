/*
  Created 6/9/12
  Last modified 6/9/12

  EL Arm with Fist Pump Counter
*/

#include <math.h>
#include <SoftwareSerial.h>

#define rxPin 255 // 255 because we don't need a receive pin
 // Connect the Arduino pin 3 to the rx pin on the 7 segment display
#define txPin 1

int counter = 0;

SoftwareSerial LEDserial=SoftwareSerial(rxPin, txPin);

int xAxisPin = A0;
int yAxisPin = A2;
int zAxisPin = A3;
//int GS1Pin = ;
//int GS2Pin = ;
//int sleepPin = ;

int triacPin = 5;

float Vsupply = 3.3;
float Vref = 3.3;

int valx;
int valy;
int valz;
float dval;
float dval_avg;

int pulse_width = 20;
int brightness = 0;
float testVal;

long time;
long interval = 10;

const int numReadings = 4;
float readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
float total = 0;                  // the running total
int average = 0;              // the average

float dval_decay = 0;
float T = 150;
float a = -2*512./pow(T,2);
int n = 0;


void setup() {
  pinMode(txPin, OUTPUT);
  LEDserial.begin(9600);
  delay(100);
  time = millis();
  pinMode(triacPin,OUTPUT);
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
  
  LEDserial.print("v");
  delay(5);
  LEDserial.print("w"); // send the command char for decimal control
  LEDserial.write(B10000000);
  LEDserial.print("0000");
}


void loop() {
  
  // Sample accelerometer
  if ( (millis()-time) > interval ) {
    time = millis();
    
    total = total - readings[index];
    
    // Read and process values from accelerometer
    valx = analogRead(xAxisPin);
    valy = analogRead(yAxisPin);
    valz = analogRead(zAxisPin);
    dval = sqrt(  pow(abs(valx-512),2) + pow(abs(valy-512),2) + pow(abs(valz-512),2)  );

    readings[index] = dval;
    total = total + readings[index];
    index++;
    if (index >= numReadings)
      index = 0;
    dval_avg = total / numReadings;


    if (dval_avg > dval_decay) {
      dval_decay = dval_avg;
      n = 0;
      if(dval_avg > 200){
        counter++;
        LEDserial.print("v");
        delay(5);
        LEDserial.print("w"); // send the command char for decimal control
        LEDserial.write(B10000000);
        if(counter<10) LEDserial.print("000");
        if(counter<100&&counter>=10) LEDserial.print("00");
        if(counter<1000&&counter>=100) LEDserial.print("0");
        LEDserial.print(counter);
//        Serial.print("counter ");
//        Serial.println(counter);
      }
    }
    
    brightness = dval_decay*20./512. + 3.;
    if (brightness > 20) {
      brightness = 20;
    }
    
//    Serial.println(dval_decay);
//    Serial.println(brightness);
//    Serial.println();

  }
  
  // Control EL through triac
  digitalWrite(triacPin,HIGH);
  delay(brightness);
  digitalWrite(triacPin,LOW);
  delay(pulse_width-brightness);
  
  n++;
  dval_decay = dval_decay + a/2.*pow(interval,2)*(2.*n-1.);
}
