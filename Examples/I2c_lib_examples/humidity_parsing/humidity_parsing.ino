#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include <Ezo_i2c_util.h> //brings in common print statements

Ezo_board HUM = Ezo_board(111, "HUM");       //create a HUM circuit object, who's address is 111 and name is "HUM"

char Humidity_data[32];          //we make a 32-byte character array to hold incoming data from the Humidity sensor.
char *HUMID;                     //char pointer used in string parsing.
char *TMP;                       //char pointer used in string parsing.
char *NUL;                       //char pointer used in string parsing (the sensor outputs some text that we don't need).
char *DEW;                       //char pointer used in string parsing.

float HUMID_float;               //float var used to hold the float value of the humidity.
float TMP_float;                 //float var used to hold the float value of the temperature.
float DEW_float;                 //float var used to hold the float value of the dew point.

void step1();  //forward declarations of functions to use them in the sequencer before defining them
void step2();

Sequencer2 Seq(&step1, 1000, &step2, 0);  //calls the steps in sequence with time in between them

// enable pins for use with WIFI hydroponics kit v1.5, remove them if you aren't using a wifi kit
// older models may use different pins
const int EN_1 = 12;
const int EN_2 = 27;
const int EN_3 = 15;
const int EN_4 = 33;

void setup() {
  pinMode(EN_1, OUTPUT);                                                         //set enable pins as outputs
  pinMode(EN_2, OUTPUT);
  pinMode(EN_3, OUTPUT);
  pinMode(EN_4, OUTPUT);
  digitalWrite(EN_1, LOW);                                                       //set enable pins to enable the circuits
  digitalWrite(EN_2, LOW);
  digitalWrite(EN_3, HIGH);
  digitalWrite(EN_4, LOW);
  
  Serial.begin(9600);           //enable serial port.
  Wire.begin();                 //enable I2C port.
  Seq.reset();                  //initialize the sequencer
  delay(3000);
  HUM.send_cmd("o,t,1");        //send command to enable temperature output
  delay(300);
  HUM.send_cmd("o,dew,1");      //send command to enable dew point output
  delay(300);
}

void loop() {
  Seq.run();                    //run the sequncer to do the polling
}

void step1() {
  //send a read command using send_cmd because we're parsing it ourselves
  HUM.send_cmd("r");
}

void step2() {

  //uncomment this line to print the answers without parsing
  /*receive_and_print_response(HUM);*/

  HUM.receive_cmd(Humidity_data, 32);       //put the response into the buffer

  HUMID = strtok(Humidity_data, ",");       //let's pars the string at each comma.
  TMP = strtok(NULL, ",");                  //let's pars the string at each comma.
  NUL = strtok(NULL, ",");                  //let's pars the string at each comma (the sensor outputs the word "DEW" in the string, we dont need it.
  DEW = strtok(NULL, ",");                  //let's pars the string at each comma.

  print_device_info(HUM);                //print our boards name and address
  Serial.print(" - Hum: ");                      //we now print each value we parsed separately.
  Serial.print(HUMID);                     //this is the humidity value.

  Serial.print(" Air Temp: ");                  //we now print each value we parsed separately.
  Serial.print(TMP);                       //this is the air temperature value.

  Serial.print(" Dew: ");                      //we now print each value we parsed separately.
  Serial.println(DEW);                       //this is the dew point.
  
  //uncomment this section if you want to take the values and convert them into floating point number.
  /*
     HUMID_float=atof(HUMID);
     TMP_float=atof(TMP);
     DEW_float=atof(DEW);
  */
}
