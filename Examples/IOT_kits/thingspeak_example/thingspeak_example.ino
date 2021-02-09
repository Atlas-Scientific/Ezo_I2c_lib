#include <Ezo_i2c.h>                                             //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>                                                //include Arduinos i2c library
#include <ESP8266WiFi.h>                                         //include esp8266 wifi library 
#include "ThingSpeak.h"                                          //include thingspeak library

#include <sequencer3.h>                                          //imports a 3 function sequencer 
#include <sequencer1.h>                                          //imports a 1 function sequencer 
#include <Ezo_i2c_util.h>                                        //brings in common print statements

WiFiClient  client;                                              //declare that this device connects to a Wi-Fi network,create a connection to a specified internet IP address


//----------------Fill in your Wi-Fi / ThingSpeak Credentials-------
const String ssid = "Wifi Name";                                 //The name of the Wi-Fi network you are connecting to
const String pass = "Wifi Password";                             //Your WiFi network password
const long myChannelNumber = 1234566;                            //Your Thingspeak channel number
const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX";                 //Your ThingSpeak Write API Key
//------------------------------------------------------------------

Ezo_board PH = Ezo_board(99, "PH");                              //create a pH circuit object, who's I2C address is 99 and name is "PH"
Ezo_board EC = Ezo_board(100, "EC");                             //create a EC circuit object, who's I2C address is 100 and name is "EC"
Ezo_board RTD = Ezo_board(102, "RTD");                           //create an RTD circuit object who's I2C address is 102 and name is "RTD"

//enable pins for each circuit
const int EN_PH = 14;
const int EN_EC = 12;
const int EN_RTD = 15;

bool wifi_isconnected(){                        //function to check if wifi is connected
  return (WiFi.status() == WL_CONNECTED);
}

void reconnect_wifi(){                                //function to reconnect wifi if its not connected
  if(!wifi_isconnected()){
    WiFi.begin(ssid, pass);
    Serial.println("connecting to wifi");
  }
}

void thingspeak_send(){                                           //if we're datalogging
  if(wifi_isconnected()){
    int return_code = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); 
    if (return_code == 200) {                                                          //code for successful transmission
        Serial.println("sent to thingspeak");
    }else{
      Serial.println("couldnt send to thingspeak");
    }
  }
}

void step1();    //forward declarations of functions to use them in the sequencer before defining them
void step2();
void step3();

const unsigned long thingspeak_delay = 15000;            //how long we wait to send values to thingspeak, in milliseconds
const unsigned long reading_delay = 815;                 //how long we wait to receive a response, in milliseconds 

Sequencer3 Seq(&step1, reading_delay,  //calls the steps in sequence with time in between them
               &step2, reading_delay,  
               &step3, 2000);

Sequencer1 Wifi_Seq( &reconnect_wifi, 10000);   //calls the wifi reconnect function every 10 seconds

Sequencer1 Thingspeak_seq( &thingspeak_send, thingspeak_delay);    //calls the function to send data to thingspeak every [thingspeak_delay] milliseconds

void setup() {                                                                    //set up the hardware
  pinMode(EN_PH, OUTPUT);                                                         //set enable pins as outputs
  pinMode(EN_EC, OUTPUT);
  pinMode(EN_RTD, OUTPUT);
  digitalWrite(EN_PH, LOW);                                                       //set enable pins to enable the circuits
  digitalWrite(EN_EC, LOW);
  digitalWrite(EN_RTD, HIGH);
  
  Wire.begin();                                                                   //enable I2C port
  Serial.begin(9600);                                                             //enable serial port, set baud rate to 9600
  WiFi.mode(WIFI_STA);                                                            //set ESP8266 mode as a station to be connected to wifi network
  ThingSpeak.begin(client);                                                       //enable ThingSpeak connection

  Wifi_Seq.reset();                                                               //initialize the sequencers
  Seq.reset();
  Thingspeak_seq.reset();
}

void loop() {
  Wifi_Seq.run();
  Seq.run();                                                                      //run the sequncer to do the polling
  Thingspeak_seq.run();
}

void step1() {
  //send a read command. we use this command instead of RTD.send_cmd("R"); 
  //to let the library know to parse the reading
  RTD.send_read_cmd();
}

void step2() {
  receive_and_print_reading(RTD);             //get the reading from the RTD circuit

  if ((RTD.get_error() == Ezo_board::SUCCESS) && (RTD.get_last_received_reading() > -1000.0)) { //if the temperature reading has been received and it is valid
    PH.send_read_with_temp_comp( RTD.get_last_received_reading());
    EC.send_read_with_temp_comp( RTD.get_last_received_reading());
    ThingSpeak.setField(3, String(RTD.get_last_received_reading(), 2));                 //assign temperature readings to the third column of thingspeak channel
  } else {                                                                                      //if the temperature reading is invalid
    PH.send_read_with_temp_comp(25.0);
    EC.send_read_with_temp_comp(25.0);      
    ThingSpeak.setField(3, String(25.0, 2));                 //assign temperature readings to the third column of thingspeak channel
  }

  Serial.print(" ");
}

void step3() {
  receive_and_print_reading(PH);             //get the reading from the PH circuit
  if (PH.get_error() == Ezo_board::SUCCESS) {                                          //if the PH reading was successful (back in step 1)
     ThingSpeak.setField(1, String(PH.get_last_received_reading(), 2));                 //assign PH readings to the first column of thingspeak channel
  }
  Serial.print("  ");
  receive_and_print_reading(EC);             //get the reading from the EC circuit
  if (EC.get_error() == Ezo_board::SUCCESS) {                                          //if the EC reading was successful (back in step 1)
     ThingSpeak.setField(2, String(EC.get_last_received_reading(), 0));                 //assign EC readings to the second column of thingspeak channel
  }

  Serial.println();
}
