#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library

Ezo_board EC = Ezo_board(100, "EC");       //create a EC circuit object, who's address is 100 and name is "EC"
Ezo_board RTD = Ezo_board(102, "RTD");      //create an RTD circuit object who's address is 102 and name is "RTD"

bool reading_request_phase = true;        //selects our phase

uint32_t next_poll_time = 0;              //holds the next time we receive a response, in milliseconds
const unsigned int response_delay = 1000; //how long we wait to receive a response, in milliseconds

float ec;         //used to hold floating point number that is ec
float temp;       //used to hold floating point number that is temperature

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                   //start the serial communication to the computer
}

void receive_reading(Ezo_board &Sensor) {               // function to decode the reading after the read command was issued

  Serial.print(Sensor.get_name()); Serial.print(": ");  // print the name of the circuit getting the reading

  Sensor.receive_read();              //get the response data and put it into the [Sensor].reading variable if successful

  switch (Sensor.get_error()) {             //switch case based on what the response code is.
    case Ezo_board::SUCCESS:
      /*                                                            //uncomment this section for access to sensor readings
      if (Sensor.get_name()=="EC"){ec = Sensor.get_reading();}      //the EC readings are now stored in a float
      if (Sensor.get_name()=="RTD"){temp = Sensor.get_reading();}   //the temperature readings are now stored in a float
      */
      Serial.print(Sensor.get_reading(), 3);  //the command was successful, print the reading
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
    if((RTD.get_error() == Ezo_board::SUCCESS) && (RTD.get_reading() > -1000.0)){
      EC.send_read_with_temp_comp(RTD.get_reading());
    }else{
       EC.send_read_with_temp_comp(25.0);
    }
    RTD.send_read();

    next_poll_time = millis() + response_delay; //set when the response will arrive
    reading_request_phase = false;       //switch to the receiving phase
  }
  else {                               //if were in the receiving phase
    if (millis() >= next_poll_time) {  //and its time to get the response

      receive_reading(EC);             //get the reading from the PH circuit
      Serial.print("  ");
      receive_reading(RTD);             //get the reading from the RTD circuit
      Serial.println();

      reading_request_phase = true;            //switch back to asking for readings
    }
  }
}
