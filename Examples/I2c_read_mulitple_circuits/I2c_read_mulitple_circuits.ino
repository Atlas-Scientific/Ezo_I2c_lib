#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library

Ezo_board PH = Ezo_board(99, "PH");       //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"

bool reading_request_phase = true;        //selects our phase

uint32_t next_poll_time = 0;              //holds the next time we receive a response, in milliseconds
const unsigned int response_delay = 1000; //how long we wait to receive a response, in milliseconds

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                   //start the serial communication to the computer
}

void receive_reading(Ezo_board &Sensor) {               // function to decode the reading after the read command was issued
  
  Serial.print(Sensor.get_name()); Serial.print(": "); // print the name of the circuit getting the reading
  
  Sensor.receive_read();              //get the response data and put it into the [Sensor].reading variable if successful
                                      
  switch (Sensor.get_error()) {             //switch case based on what the response code is.
    case Ezo_board::SUCCESS:        
      Serial.print(Sensor.get_reading());   //the command was successful, print the reading
      break;

    case Ezo_board::FAIL:          
      Serial.print("Failed ");        //means the command has failed.
      break;  

    case Ezo_board::NOT_READY:      
      Serial.print("Pending ");       //the command has not yet been finished calculating.
      break;

    case Ezo_board::NO_DATA:      
      Serial.print("No Data ");       //the sensor has no data to send.
      break;
  }
}

void loop() {
  if (reading_request_phase) {           //if were in the phase where we ask for a reading

    //send a read command. we use this command instead of PH.send_cmd("R"); 
    //to let the library know to parse the reading
    PH.send_read();                      
    EC.send_read();
    
    next_poll_time = millis() + response_delay; //set when the response will arrive
    reading_request_phase = false;       //switch to the receiving phase
  }
  else {                               //if were in the receiving phase
    if (millis() >= next_poll_time) {  //and its time to get the response

      receive_reading(PH);             //get the reading from the PH circuit
      Serial.print("  ");
      receive_reading(EC);             //get the reading from the EC circuit
      Serial.println();

      reading_request_phase = true;            //switch back to asking for readings
    }
  }
}
