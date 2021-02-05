/*This code was written for the Instructable "DIY PH DOSING PUMP" (Link:https://www.instructables.com/id/DIY-PH-DOSING-PUMP/) 
  and hackster.io "DIY pH Dosing Pump" (Link: https://www.hackster.io/atlas-scientific/diy-ph-dosing-pump-061cef)
  It was tested on an Arduino UNO.
  Real time pH monitoring is done using the EZO pH sensor and two EZO PMP. The pumps dispense pH UP and pH DOWN solutions into the sample and they are triggered in accordance with the current pH reading.
  The goal is to maintain the pH level of the sample between 8 and 8.5
  The sensors must be calibrated and switched to I2C mode before using this code. The ability to send commands to the sensors is not incorporated here.
  After uploading the code to your arduino, open the serial monitor, set the baud rate to 9600 and select "Carriage return". The pH dosing system is now active.*/


#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include <Ezo_i2c_util.h> //brings in common print statements

Ezo_board PH = Ezo_board(99, "PH");                            //creat a PH circuit object, whose address is 99 and name is "PH"
Ezo_board PMP_UP = Ezo_board(103, "PMP_UP");                   //create a pump circuit object, whose address is 103 and name is "PMP_UP". This pump dispenses pH up solution.
Ezo_board PMP_DOWN = Ezo_board(104, "PMP_DOWN");               //create a pump circuit object, whose address is 104 and name is "PMP_DOWN". This pump dispenses pH down solution.


void step1();  //forward declarations of functions to use them in the sequencer before defining them
void step2();

Sequencer2 Seq(&step1, 1000, &step2, 0); //calls the steps in sequence with time in between them

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
  Seq.reset();                            //initialize the sequencer
}

void loop() {
  Seq.run();                              //run the sequncer to do the polling
}

void step1(){
   //send a read command. we use this command instead of PH.send_cmd("R"); 
  //to let the library know to parse the reading
  PH.send_read_cmd();      
}

void step2(){
  
  receive_and_print_reading(PH);             //get the reading from the PH circuit

  Serial.print("    ");
  if (PH.get_error() == Ezo_board::SUCCESS){
    if (PH.get_last_received_reading() <= 8) {                            //test condition against pH reading
      Serial.println("PH LEVEL LOW,PMP_UP = ON");
      PMP_UP.send_cmd_with_num("d,", 0.5);                  //if condition is true, send command to turn on pump (called PMP_UP) and dispense pH up solution, in amounts of 0.5ml. Pump turns clockwise.
    }
    else {
      PMP_UP.send_cmd("x");                                 //if condition is false, send command to turn off pump (called PMP_UP)
    }
  
    if (PH.get_last_received_reading() >= 8.5) {                          //test condition against pH reading
      Serial.println("PH LEVEL HIGH,PMP_DOWN = ON");
      PMP_DOWN.send_cmd_with_num("d,", -0.5);               //if condition is true, send command to turn on pump (called PMP_DOWN) and dispense pH down solution, in amounts of 0.5ml. Pump turns counter-clockwise.
    }
    else {
      PMP_DOWN.send_cmd("x");                               //if condition is false, send command to turn off pump (called PMP_DOWN)
    }
  }
  Serial.println();
}
