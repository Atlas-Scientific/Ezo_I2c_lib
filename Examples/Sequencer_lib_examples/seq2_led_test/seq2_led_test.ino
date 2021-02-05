/*
 * example code for the sequencer library
 * This example blinks an LED without using blocking statements like delay.
 * It lets you run other code while the LEDs blink as long as the Seq.run()
 * function is called frequently
 * 
 */

#include <sequencer2.h> //imports a sequencer that calls two functions 

const int LED_PIN = 0;

void Led_on(){
  digitalWrite(LED_PIN, HIGH);
}

void Led_off(){
  digitalWrite(LED_PIN, LOW);
  Serial.println("off");
}

//creates a sequencer object that calls the functions Led_on and Led_off
//after waiting 1000 milliseconds, or 1 second, in between
Sequencer2 Seq(&Led_on, 1000, &Led_off, 1000);  


void setup() {
  pinMode(LED_PIN, OUTPUT);
  Seq.reset();    //initializes the sequencer to start from the first step and trigger right away
}

void loop() {
  Seq.run();     //runs the sequencer, calling the two functions we passed in after the time we set has passed
}
