/*This code was written for the Instructable "DIY PH DOSING PUMP" (Link:https://www.instructables.com/id/DIY-PH-DOSING-PUMP/) 
  and hackster.io "DIY pH Dosing Pump" (Link: https://www.hackster.io/atlas-scientific/diy-ph-dosing-pump-061cef)
  It was tested on an Arduino UNO.
  Real time pH monitoring is done using the EZO pH sensor and two EZO PMP. The pumps dispense pH UP and pH DOWN solutions into the sample and they are triggered in accordance with the current pH reading.
  The goal is to maintain the pH level of the sample between 8 and 8.5
  The sensors must be calibrated and switched to I2C mode before using this code. The ability to send commands to the sensors is not incorporated here.
  After uploading the code to your arduino, open the serial monitor, set the baud rate to 115200 and select "Carriage return". The pH dosing system is now active.*/


#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library

Ezo_board PH = Ezo_board(99, "PH");                            //creat a PH circuit object, whose address is 99 and name is "PH"
Ezo_board PMP_UP = Ezo_board(103, "PMP_UP");                   //create a pump circuit object, whose address is 103 and name is "PMP_UP". This pump dispenses pH up solution.
Ezo_board PMP_DOWN = Ezo_board(104, "PMP_DOWN");               //create a pump circuit object, whose address is 104 and name is "PMP_DOWN". This pump dispenses pH down solution.

bool reading_request_phase = true;                             //selects our phase

uint32_t next_poll_time = 0;                                   //holds the next time we receive a response, in milliseconds
const unsigned int response_delay = 1000;                      //how long we wait to receive a response, in milliseconds

void setup() {
  Wire.begin();                                                //start the I2C
  Serial.begin(115200);                                        //start the serial communication to the computer
}

void receive_reading(Ezo_board &Sensor) {                      // function to decode the reading after the read command was issued

  Serial.print(Sensor.get_name()); Serial.print(": ");         // print the name of the circuit getting the reading

  Sensor.receive_read();                                       //get the response data and put it into the [Sensor].reading variable if successful

  switch (Sensor.get_error()) {                                //switch case based on what the response code is.
    case Ezo_board::SUCCESS:
      Serial.print(Sensor.get_reading());                      //the command was successful, print the reading
      break;

    case Ezo_board::FAIL:
      Serial.print("Failed ");                                 //means the command has failed.
      break;

    case Ezo_board::NOT_READY:
      Serial.print("Pending ");                                //the command has not yet been finished calculating.
      break;

    case Ezo_board::NO_DATA:
      Serial.print("No Data ");                                //the sensor has no data to send.
      break;
  }
}

void loop() {
  if (reading_request_phase) {                                 //if were in the phase where we ask for a reading

    //send a read command. we use this command instead of PH.send_cmd("R");
    //to let the library know to parse the reading
    PH.send_read();

    next_poll_time = millis() + response_delay;               //set when the response will arrive
    reading_request_phase = false;                            //switch to the receiving phase
  }
  else {                                                      //if were in the receiving phase
    if (millis() >= next_poll_time) {                         //and its time to get the response

      receive_reading(PH);                                    //get the reading from the PH circuit
      Serial.print("    ");


      if (PH.get_reading() <= 8) {                            //test condition against pH reading
        Serial.println("PH LEVEL LOW,PMP_UP = ON");
        PMP_UP.send_cmd_with_num("d,", 0.5);                  //if condition is true, send command to turn on pump (called PMP_UP) and dispense pH up solution, in amounts of 0.5ml. Pump turns clockwise.
      }
      else {
        PMP_UP.send_cmd("x");                                 //if condition is false, send command to turn off pump (called PMP_UP)
      }

      if (PH.get_reading() >= 8.5) {                          //test condition against pH reading
        Serial.println("PH LEVEL HIGH,PMP_DOWN = ON");
        PMP_DOWN.send_cmd_with_num("d,", -0.5);               //if condition is true, send command to turn on pump (called PMP_DOWN) and dispense pH down solution, in amounts of 0.5ml. Pump turns counter-clockwise.
      }
      else {
        PMP_DOWN.send_cmd("x");                               //if condition is false, send command to turn off pump (called PMP_DOWN)
      }

      Serial.println();
      reading_request_phase = true;                           //switch back to asking for readings
    }
  }
}
