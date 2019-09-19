#include <Ezo_i2c.h>                                             //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>                                                //include Arduinos i2c library
Ezo_board EC = Ezo_board(100, "EC");                             //create a EC circuit object, who's I2C address is 100 and name is "EC"
Ezo_board RTD = Ezo_board(102, "RTD");                           //create an RTD circuit object who's I2C address is 102 and name is "RTD"

enum reading_step {REQUEST_TEMP, READ_TEMP_AND_COMPENSATE, REQUEST_EC, READ_RESPONSE };          //the readings are taken in 3 steps
                                                                                  //step 1 tell the temp sensor to take a reading
                                                                                  //step 2 consume the temp reading and send it to EC
                                                                                  //step 4 tell the EC to take a reading based on the temp reading we just received 
                                                                                  //step 3 consume the EC readings

enum reading_step current_step = REQUEST_TEMP;                                    //the current step keeps track of where we are. lets set it to REQUEST_TEMP (step 1) on startup     

unsigned long next_step_time = 0;                                                 //holds the time in milliseconds. this is used so we know when to move between the 3 steps    
const unsigned long reading_delay = 815;                                          //how long we wait to receive a response, in milliseconds 
const unsigned int temp_delay = 300;
const unsigned int loop_delay = 5000;

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
}

void loop() {
  switch(current_step) {                                                          //selects what to do based on what reading_step we are in
//------------------------------------------------------------------
 
    case REQUEST_TEMP:                                                            //when we are in the first step
      if (millis() >= next_step_time) {                                           //check to see if enough time has past, if it has 
        RTD.send_read_cmd();
        next_step_time = millis() + reading_delay; //set when the response will arrive
        current_step = READ_TEMP_AND_COMPENSATE;       //switch to the receiving phase
      }
      break;
//------------------------------------------------------------------
    case READ_TEMP_AND_COMPENSATE:
      if (millis() >= next_step_time) {
        
        RTD.receive_read_cmd();                                                   //get the temp reading  
        Serial.print(RTD.get_name()); Serial.print(": ");                         //print the name of the circuit we just got a reading from
        
        if ((reading_succeeded(RTD) == true) && (RTD.get_last_received_reading() > -1000.0)) { //if the temperature reading has been received and it is valid
          EC.send_cmd_with_num("T,", RTD.get_last_received_reading());
        } else {                                                                                      //if the temperature reading is invalid
          EC.send_cmd_with_num("T,", 25.0);                                                          //send default temp = 25 deg C to EC sensor
        }

        if(RTD.get_error() == Ezo_board::SUCCESS){                                            //if the RTD reading was successful
          Serial.print(RTD.get_last_received_reading(), 1);                                   //print the reading (with 1 decimal places)
        }
        
        Serial.print(" ");    
        next_step_time = millis() + temp_delay; //set when the response will arrive
        current_step = REQUEST_EC;       //switch to the receiving phase
      }
      break;
      
    case REQUEST_EC:
      if (millis() >= next_step_time) {
        EC.send_read_cmd();
        next_step_time = millis() + reading_delay;                                           //advance the next step time by adding the delay we need for the sensor to take the reading
        current_step = READ_RESPONSE;                                                              //switch to the next step
      }
      break;
//------------------------------------------------------------------

    case READ_RESPONSE:                             //if were in the receiving phase
      if (millis() >= next_step_time) {  //and its time to get the response
        Serial.print(" ");
         
        EC.receive_read_cmd();                                                                //get the EC reading 
        Serial.print(EC.get_name()); Serial.print(": ");                                      //print the name of the circuit we just got a reading from
        
        if(reading_succeeded(EC) == true){                                                    //if the EC reading has been received and it is valid
          Serial.print(EC.get_last_received_reading(), 0);                                    //print the reading (with 0 decimal places)
        }

        Serial.println();
        next_step_time =  millis() + loop_delay;                                              //update the time for the next reading loop 
        current_step = REQUEST_TEMP;                                                          //switch back to asking for readings
      }
      break;
  }
}

bool reading_succeeded(Ezo_board &Sensor) {                                                 //this function makes sure that when we get a reading we know if it was valid or if we got an error 

  switch (Sensor.get_error()) {                                                             //switch case based on what the response code was
    case Ezo_board::SUCCESS:                                                                //if the reading was a success
      return true;                                                                          //return true, the reading succeeded 
      
    case Ezo_board::FAIL:                                                                   //if the reading faild
      Serial.print("Failed ");                                                              //print "failed"
      return false;                                                                         //return false, the reading was not successful

    case Ezo_board::NOT_READY:                                                              //if the reading was taken to early, the command has not yet finished calculating
      Serial.print("Pending ");                                                             //print "Pending"
      return false;                                                                         //return false, the reading was not successful

    case Ezo_board::NO_DATA:                                                                //the sensor has no data to send
      Serial.print("No Data ");                                                             //print "no data"
      return false;                                                                         //return false, the reading was not successful
      
    default:                                                                                //if none of the above happened
     return false;                                                                          //return false, the reading was not successful
  }
} 