
#include <iot_cmd.h>
#include <sequencer2.h>                                          //imports a 4 function sequencer 
#include <Ezo_i2c_util.h>                                        //brings in common print statements
#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library

Ezo_board PMP1 = Ezo_board(56, "PMP1");    //create an PMP circuit object who's address is 56 and name is "PMP1"
Ezo_board PMP2 = Ezo_board(57, "PMP2");    //create an PMP circuit object who's address is 57 and name is "PMP2"
Ezo_board PMP3 = Ezo_board(58, "PMP3");    //create an PMP circuit object who's address is 58 and name is "PMP3"

Ezo_board device_list[] = {               //an array of boards used for sending commands to all or specific boards
  PMP1,
  PMP2,
  PMP3
};

Ezo_board* default_board = &device_list[0]; //used to store the board were talking to

//gets the length of the array automatically so we dont have to change the number every time we add new boards
const uint8_t device_list_len = sizeof(device_list) / sizeof(device_list[0]);

const unsigned long reading_delay = 1000;                 //how long we wait to receive a response, in milliseconds
unsigned int poll_delay = 2000 - reading_delay;

void step1();      //forward declarations of functions to use them in the sequencer before defining them
void step2();

Sequencer2 Seq(&step1, reading_delay,   //calls the steps in sequence with time in between them
               &step2, poll_delay);

bool polling = true;                                     //variable to determine whether or not were polling the circuits

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
  Seq.reset();
}

void loop() {
  String cmd;                             //variable to hold commands we send to the kit

  if (receive_command(cmd)) {            //if we sent the kit a command it gets put into the cmd variable
    polling = false;                     //we stop polling
    if (!process_coms(cmd)) {            //then we evaluate the cmd for kit specific commands
      process_command(cmd, device_list, device_list_len, default_board);    //then if its not kit specific, pass the cmd to the IOT command processing function
    }
  }

  if (polling == true) {                 //if polling is turned on, run the sequencer
    Seq.run();
  }
  delay(50);
}

void step1() {
  //send a read command. we use this command instead of PMP1.send_cmd("R");
  //to let the library know to parse the reading
  PMP1.send_read_cmd();
  PMP2.send_read_cmd();
  PMP3.send_read_cmd();
}

void step2() {
  receive_and_print_reading(PMP1);             //get the reading from the PMP1 circuit
  Serial.print("  ");
  receive_and_print_reading(PMP2);
  Serial.print("  ");
  receive_and_print_reading(PMP3);
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

      float mintime = reading_delay;
      if (new_delay >= (mintime / 1000.0)) {                                     //make sure its greater than our minimum time
        Seq.set_step2_time((new_delay * 1000.0) - reading_delay);          //convert to milliseconds and remove the reading delay from our wait
      } else {
        Serial.println("delay too short");                          //print an error if the polling time isnt valid
      }
    }
    return true;
  }
  return false;                         //return false if the command is not in the list, so we can scan the other list or pass it to the circuit
}

void print_help() {
  Serial.println(F("Atlas Scientific Tri PMP sample code                                       "));
  Serial.println(F("Commands:                                                                  "));
  Serial.println(F("poll         Takes readings continuously of all sensors                    "));
  Serial.println(F("                                                                           "));
  Serial.println(F("PMP[N]:[query]       issue a query to a pump named PMP[N]                  "));
  Serial.println(F("  ex: PMP2:status    sends the status command to pump named PMP2           "));
  Serial.println(F("      PMP1:d,100     requests that PMP1 dispenses 100ml                    "));
  Serial.println();
  Serial.println(F("      The list of all pump commands is available in the Tri PMP datasheet  "));
  Serial.println();
  iot_cmd_print_listcmd_help();
  Serial.println();
  iot_cmd_print_allcmd_help();
}
