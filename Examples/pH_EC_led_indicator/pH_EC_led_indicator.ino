/*This code was written for the Instructable "MAKE YOUR OWN PH AND SALINITY MONITORING SYSTEM WITH LED INDICATORS" (Link: https://www.instructables.com/id/MAKE-YOUR-OWN-PH-EC-MONITOR-WITH-LED-INDICATORS/)
and hackster.io "Make Your Own pH and Salinity Monitoring System" (Link: https://www.hackster.io/atlas-scientific/make-your-own-ph-and-salinity-monitoring-system-fb14c1)
Testing was done using an Arduino UNO.
The code allows you to monitor in real-time, pH and EC. You can modify it to observe other parameters such as DO, temperature and ORP.  It works 
with Atlas Scientific's EZO circuits. The sensors must be calibrated and switched to I2C mode before using this code as it does not have the capability 
of allowing the user to send commands to the circuits. The readings of the sensors are displayed on the Arduino serial monitor. 
There are two LEDs which functions as a warning system. They are turned on when the readings go out of the defined limits.
These LEDs offer a simple demonstration of how you can utilize sensor readings to trigger other hardware.
Once you have uploaded the code to your Arduino, open the serial monitor, set the baud rate to 9600 and append "Carriage return"*/

#include <Ezo_i2c.h> //include the EZO I2C library (EZO_i2c.h is customized header file for Atlas Scientific's EZO circuits in I2C mode. Link: https://github.com/Atlas-Scientific/Ezo_I2c_lib)
#include <Wire.h>    //include arduinos i2c library

Ezo_board PH = Ezo_board(99, "PH");       //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"
int PH_led = 10;                           //define pin for pH led
int EC_led = 9;                           //define pin for EC led

bool reading_request_phase = true;        //selects our phase

uint32_t next_poll_time = 0;              //holds the next time we receive a response, in milliseconds
const unsigned int response_delay = 1000; //how long we wait to receive a response, in milliseconds

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer at baud rate of 9600
  pinMode(PH_led, OUTPUT);                //set pin of pH led as output
  pinMode(EC_led, OUTPUT);                //set pin for EC led as output
}

void receive_reading(Ezo_board &Sensor) {               // function to decode the reading after the read command was issued

  Serial.print(Sensor.get_name()); Serial.print(": ");  // print the name of the circuit getting the reading

  Sensor.receive_read();                                //get the response data and put it into the [Sensor].reading variable if successful

  switch (Sensor.get_error()) {                         //switch case based on what the response code is.
    case Ezo_board::SUCCESS:
      Serial.print(Sensor.get_reading());               //the command was successful, print the reading
      break;

    case Ezo_board::FAIL:
      Serial.print("Failed ");                          //means the command has failed.
      break;

    case Ezo_board::NOT_READY:
      Serial.print("Pending ");                         //the command has not yet been finished calculating.
      break;

    case Ezo_board::NO_DATA:
      Serial.print("No Data ");                         //the sensor has no data to send.
      break;
  }
}

void loop() {
  if (reading_request_phase) {                          //if were in the phase where we ask for a reading

    //send a read command. we use this command instead of PH.send_cmd("R");
    //to let the library know to parse the reading
    PH.send_read();
    EC.send_read();

    next_poll_time = millis() + response_delay;         //set when the response will arrive
    reading_request_phase = false;                      //switch to the receiving phase
  }
  else {                                                //if were in the receiving phase
    if (millis() >= next_poll_time) {                   //and its time to get the response

      receive_reading(PH);                              //get the reading from the PH circuit
      if(PH.get_reading() > 10) {                       //test condition against pH reading
        digitalWrite(PH_led,HIGH);                      //if condition true, led on
      }
      else{
        digitalWrite(PH_led,LOW);                       //if condition false, led off
      }
      Serial.print("  ");

      receive_reading(EC);                              //get the reading from the EC circuit
      if (EC.get_reading() > 500.00) {                  //test condition against EC reading
        digitalWrite(EC_led,HIGH);                      //if condition true, led on
      }
       else{
        digitalWrite(EC_led,LOW);                       //if condition false, led off
       }    
      Serial.println();

      reading_request_phase = true;                     //switch back to asking for readings
    }
  }
}

