#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library
#include <sequencer3.h> //imports a 3 function sequencer
#include <Ezo_i2c_util.h> //brings in common print statements

Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"
Ezo_board RTD = Ezo_board(102, "RTD");    //create an RTD circuit object who's address is 102 and name is "RTD"

void step1(); //forward declarations of functions to use them in the sequencer before defining them
void step2();
void step3();

Sequencer3 Seq( &step1, 815, //calls the steps in sequence with time in between them
                &step2, 815, 
                &step3, 2000); 

void setup() {      
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
  Seq.reset();                            //initialize the sequencer
}

void loop() {
  Seq.run();                              //run the sequncer to do the polling
}

void step1() {
  //send a read command. we use this command instead of RTD.send_cmd("R"); 
  //to let the library know to parse the reading
  RTD.send_read_cmd();
}

void step2() {
  receive_and_print_reading(RTD);             //get the reading from the RTD circuit

  if ((RTD.get_error() == Ezo_board::SUCCESS) && (RTD.get_last_received_reading() > -1000.0)) {        //if the temperature reading has been received and it is valid
    EC.send_read_with_temp_comp(RTD.get_last_received_reading());                               //send readings from temp sensor to EC sensor
  } else {                                                                                      //if the temperature reading is invalid
    EC.send_read_with_temp_comp(25.0);                                                          //send default temp = 25 deg C to EC sensor
  }
  Serial.print(" ");
}

void step3() {
  receive_and_print_reading(EC);               //get the reading from the EC circuit and print it
  Serial.println();
}
