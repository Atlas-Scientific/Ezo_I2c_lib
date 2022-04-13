#include <iot_cmd.h>
#include <sequencer4.h>                                          //imports a 4 function sequencer 
#include <sequencer1.h>                                          //imports a 1 function sequencer 
#include <Ezo_i2c_util.h>                                        //brings in common print statements
#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library

Ezo_board PH = Ezo_board(99, "PH");       //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"
Ezo_board RTD = Ezo_board(102, "RTD");    //create an RTD circuit object who's address is 102 and name is "RTD"

Ezo_board device_list[] = {   //an array of boards used for sending commands to all or specific boards
  PH,
  EC,
  RTD
};

Ezo_board* default_board = &device_list[0]; //used to store the board were talking to

//gets the length of the array automatically so we dont have to change the number every time we add new boards
const uint8_t device_list_len = sizeof(device_list) / sizeof(device_list[0]);

//if you're using a wifi kit, uncomment these!
//------For version 1.4 use these enable pins for each circuit------
//const int EN_PH = 13;
//const int EN_EC = 12;
//const int EN_RTD = 33;
//const int EN_AUX = 27;
//------------------------------------------------------------------

//------For version 1.5 use these enable pins for each circuit------
//const int EN_PH = 12;
//const int EN_EC = 27;
//const int EN_RTD = 15;
//const int EN_AUX = 33;
//------------------------------------------------------------------

const unsigned long reading_delay = 1000;                 //how long we wait to receive a response, in milliseconds 

unsigned int poll_delay = 100;                            //how long to wait between polls after accounting for the times it takes to send readings

float k_val = 0;                                          //holds the k value for determining what to print in the help menu

bool polling  = true;                                     //variable to determine whether or not were polling the circuits

void step1();      //forward declarations of functions to use them in the sequencer before defining them
void step2();
void step3();
void step4();
Sequencer4 Seq(&step1, reading_delay,   //calls the steps in sequence with time in between them
               &step2, 300, 
               &step3, reading_delay,
               &step4, poll_delay);

void setup() {

//if you're using a wifi kit, uncomment these!
//  pinMode(EN_PH, OUTPUT);                                                         //set enable pins as outputs
//  pinMode(EN_EC, OUTPUT);
//  pinMode(EN_RTD, OUTPUT);
//  pinMode(EN_AUX, OUTPUT);
//  digitalWrite(EN_PH, LOW);                                                       //set enable pins to enable the circuits
//  digitalWrite(EN_EC, LOW);
//  digitalWrite(EN_RTD, HIGH);
//  digitalWrite(EN_AUX, LOW);

  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer

  Seq.reset();
}

void loop() {
  String cmd;                            //variable to hold commands we send to the kit

  if (receive_command(cmd)) {            //if we sent the kit a command it gets put into the cmd variable
    polling = false;                     //we stop polling  
    if(!process_coms(cmd)){              //then we evaluate the cmd for kit specific commands
      process_command(cmd, device_list, device_list_len, default_board);    //then if its not kit specific, pass the cmd to the IOT command processing function
    }
  }
  
  if (polling == true) {                 //if polling is turned on, run the sequencer
    Seq.run();
  }
}

void step1() {
  //send a read command. we use this command instead of RTD.send_cmd("R"); 
  //to let the library know to parse the reading
  RTD.send_read_cmd();
}

void step2() {
  receive_and_print_reading(RTD);             //get the reading from the RTD circuit

  if ((RTD.get_error() == Ezo_board::SUCCESS) && (RTD.get_last_received_reading() > -1000.0)) { //if the temperature reading has been received and it is valid
    PH.send_cmd_with_num("T,", RTD.get_last_received_reading());
    EC.send_cmd_with_num("T,", RTD.get_last_received_reading());
  } else {                                                                                      //if the temperature reading is invalid
    PH.send_cmd_with_num("T,", 25.0);
    EC.send_cmd_with_num("T,", 25.0);                                                          //send default temp = 25 deg C to EC sensor
  }

  Serial.print(" ");
}

void step3() {
  //send a read command. we use this command instead of PH.send_cmd("R");
  //to let the library know to parse the reading
  PH.send_read_cmd();
  EC.send_read_cmd();
}

void step4() {
  receive_and_print_reading(PH);             //get the reading from the PH circuit
  Serial.print("  ");
  receive_and_print_reading(EC);             //get the reading from the EC circuit
  Serial.println();
}



bool process_coms(const String &string_buffer) {      //function to process commands that manipulate global variables and are specifc to certain kits
  if (string_buffer == "HELP") {
    print_help();
    return true;
  }
  else if (string_buffer.startsWith("POLL")) {
    polling = true;  
    Seq.reset();
    
    int16_t index = string_buffer.indexOf(',');                    //check if were passing a polling delay parameter
    if (index != -1) {                                              //if there is a polling delay
      float new_delay = string_buffer.substring(index + 1).toFloat(); //turn it into a float

      float mintime = reading_delay*2 + 300;
      if (new_delay >= (mintime/1000.0)) {                                       //make sure its greater than our minimum time
        Seq.set_step4_time((new_delay * 1000.0) - mintime);          //convert to milliseconds and remove the reading delay from our wait
      } else {
        Serial.println("delay too short");                          //print an error if the polling time isnt valid
      }
    }
    return true;
  }
  return false;                         //return false if the command is not in the list, so we can scan the other list or pass it to the circuit
}

void get_ec_k_value() {                                   //function to query the value of the ec circuit
  char rx_buf[10];                                        //buffer to hold the string we receive from the circuit
  EC.send_cmd("k,?");                                     //query the k value
  delay(300);
  if (EC.receive_cmd(rx_buf, 10) == Ezo_board::SUCCESS) { //if the reading is successful
    k_val = String(rx_buf).substring(3).toFloat();        //parse the reading into a float
  }
}

void print_help() {
  
  get_ec_k_value();
  Serial.println(F("Atlas Scientific I2C Communications Sample Code                            "));
  Serial.println(F("Commands:                                                                  "));
  Serial.println(F("poll         Takes readings continuously of all sensors                    "));
  Serial.println(F("                                                                           "));
  Serial.println(F("ph:cal,mid,7     calibrate to pH 7                                         "));
  Serial.println(F("ph:cal,low,4     calibrate to pH 4                                         "));
  Serial.println(F("ph:cal,high,10   calibrate to pH 10                                        "));
  Serial.println(F("ph:cal,clear     clear calibration                                         "));
  Serial.println(F("                                                                           "));
  Serial.println(F("ec:cal,dry           calibrate a dry EC probe                              "));
  Serial.println(F("ec:k,[n]             used to switch K values, standard probes values are 0.1, 1, and 10 "));
  Serial.println(F("ec:cal,clear         clear calibration                                     "));

  if (k_val > 9) {
    Serial.println(F("For K10 probes, these are the recommended calibration values:            "));
    Serial.println(F("  ec:cal,low,12880     calibrate EC probe to 12,880us                    "));
    Serial.println(F("  ec:cal,high,150000   calibrate EC probe to 150,000us                   "));
  }
  else if (k_val > .9) {
    Serial.println(F("For K1 probes, these are the recommended calibration values:             "));
    Serial.println(F("  ec:cal,low,12880     calibrate EC probe to 12,880us                    "));
    Serial.println(F("  ec:cal,high,80000    calibrate EC probe to 80,000us                    "));
  }
  else if (k_val > .09) {
    Serial.println(F("For K0.1 probes, these are the recommended calibration values:           "));
    Serial.println(F("  ec:cal,low,84        calibrate EC probe to 84us                        "));
    Serial.println(F("  ec:cal,high,1413     calibrate EC probe to 1413us                      "));
  }

  Serial.println(F("                                                                           "));
  Serial.println(F("rtd:cal,t            calibrate the temp probe to any temp value            "));
  Serial.println(F("                     t= the temperature you have chosen                    "));
  Serial.println(F("rtd:cal,clear        clear calibration                                     "));
  Serial.println();
  iot_cmd_print_listcmd_help();
  Serial.println();
  iot_cmd_print_allcmd_help();
}
