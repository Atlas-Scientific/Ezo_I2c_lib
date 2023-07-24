#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include <Ezo_i2c_util.h> //brings in common print statements

Ezo_board DO = Ezo_board(97, "DO");    //create an DO circuit object who's address is 97 and name is "DO"
Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"

char EC_data[32];          //we make a 32-byte character array to hold incoming data from the EC sensor.
char *EC_str;                     //char pointer used in string parsing.
char *TDS;                       //char pointer used in string parsing.
char *SAL;                       //char pointer used in string parsing (the sensor outputs some text that we don't need).
char *SG;                       //char pointer used in string parsing.

float EC_float;               //float var used to hold the float value of the conductivity.
float TDS_float;                 //float var used to hold the float value of the total dissolved solids.
float SAL_float;                 //float var used to hold the float value of the salinity.
float SG_float;                 //float var used to hold the float value of the specific gravity.

void step1();  //forward declarations of functions to use them in the sequencer before defining them
void step2();

Sequencer2 Seq(&step1, 1000, &step2, 300);  //calls the steps in sequence with time in between them
//NOTE: there should be a 300ms wait time before issuing the read command so the DO circuit can process the salinity compensation command

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
  EC.send_cmd("o,tds,1");        //send command to enable TDS output
  delay(300);
  EC.send_cmd("o,s,1");        //send command to enable salinity output
  delay(300);
  EC.send_cmd("o,sg,1");      //send command to enable specific gravity output
  delay(300);
}

void loop() {
  Seq.run();                    //run the sequncer to do the polling
}

void step1() {
  //send a read command using send_cmd because we're parsing it ourselves
  EC.send_cmd("r");
  //for DO we use the send_read_cmd function so the library can parse it
  DO.send_read_cmd();
}

void step2() {

  EC.receive_cmd(EC_data, 32);       //put the response into the buffer

  EC_str = strtok(EC_data, ",");       //let's parse the string at each comma.
  TDS = strtok(NULL, ",");                  //let's parse the string at each comma.
  SAL = strtok(NULL, ",");                  //let's parse the string at each comma 
  SG = strtok(NULL, ",");                  //let's parse the string at each comma.

  Serial.print("EC: ");                      //we now print each value we parsed separately.
  Serial.print(EC_str);                     //this is the EC value.

  Serial.print(" TDS: ");                  //we now print each value we parsed separately.
  Serial.print(TDS);                       //this is the TDS value.

  Serial.print(" SAL: ");                      //we now print each value we parsed separately.
  Serial.print(SAL);                       //this is the salinity point.
  
  Serial.print(" SG: ");                      //we now print each value we parsed separately.
  Serial.println(SG);                       //this is the specific gravity point.
  
  receive_and_print_reading(DO);             //get the reading from the DO circuit
  Serial.println();

  EC_float=atof(EC_str);
  DO.send_cmd_with_num("s,", EC_float);

  //uncomment this section if you want to take the values and convert them into floating point number.
  /*
     EC_float=atof(EC_str);
     TDS_float=atof(TDS);
     SAL_float=atof(SAL);
     SG_float=atof(SG);
  */
}
