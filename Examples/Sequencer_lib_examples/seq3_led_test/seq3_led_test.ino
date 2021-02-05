/*
 * example code for the sequencer library
 * This demonstrates the use of the 3 state sequencer
 * as well as modulating the sequence time from within a step
 * The example uses the third step to make the LED blink faster
 * every time it repeats
 */

#include <sequencer3.h> //imports a sequencer that calls three functions 

const int LED_PIN = 0;

void Led_on(){
  digitalWrite(LED_PIN, HIGH);
}

void Led_off(){
  digitalWrite(LED_PIN, LOW);
  Serial.println("off");
}

void modulate(); //forward decleration so the modulate function can be used by the sequencer 
                 //while referring to the sequencer itself 

//creates a sequencer object that calls the functions Led_on, Led_off, and modulate
//in sequence after waiting 1000 milliseconds, or 1 second, in between the leds
//and 100 milliseconds in between the modulate function
Sequencer3 Seq(&Led_on, 1000, &Led_off, 1000, &modulate, 100);

void modulate(){
  // refer to the sequencer time to modulate it
  Seq.set_step2_time(Seq.get_step2_time()* .9); 
  Seq.set_step1_time(Seq.get_step1_time()* .9);
  
  //reset the sequencer time when it gets too short
  if(Seq.get_step2_time() <= 10){
    Seq.set_step2_time(1000);
    Seq.set_step1_time(1000);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Seq.reset();    //initializes the sequencer to start from the first step and trigger right away
} 

void loop() {
  Seq.run(); //runs the sequencer, calling the two functions we passed in after the time we set has passed
}
