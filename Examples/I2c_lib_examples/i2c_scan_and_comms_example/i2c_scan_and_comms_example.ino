#include <iot_cmd.h>
#include <sequencer2.h>                                          //imports a 2 function sequencer 
#include <Ezo_i2c_util.h>                                        //brings in common print statements
#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library

Ezo_board Unknown_Device = Ezo_board(20, "found device");   //object which holds the device we'll find

Ezo_board *device_list[] = {               //an array of boards used for sending commands to all or specific boards
  &Unknown_Device
};

Ezo_board* default_board = device_list[0]; //used to store the board were talking to

//gets the length of the array automatically so we dont have to change the number every time we add new boards
const uint8_t device_list_len = sizeof(device_list) / sizeof(device_list[0]);

const unsigned long reading_delay = 1000;                   //how long we wait to receive a response, in milliseconds
unsigned int poll_delay = 1000;   //how long to wait between polls after accounting for the times it takes to send readings

bool polling = true;                                     //variable to determine whether or not were polling the circuits

void step1();     //forward declarations of functions to use them in the sequencer before defining them
void step2();

Sequencer2 Seq(&step1, reading_delay,   //calls the steps in sequence with time in between them
               &step2, 0 );

bool scan_devices(){
  for(uint8_t i = 20; i < 128; ++i){                        //step through the i2C addresses
    Wire.beginTransmission(i);                              //try to talk to the device over I2C
    uint8_t error = Wire.endTransmission();                 //check if communication attempt was successful
    if(error == 0){                                         //if its possible to communicate with it
      Unknown_Device.set_address(i);                        //set that to be the address
      Serial.println();
      Unknown_Device.send_cmd("I");                         //ask what kind of device it is
      delay(300);
      receive_and_print_response(Unknown_Device);           //print the response
      return true;                                          //return that we detected a device
    }
  }
  return false;
}

void setup() {

  Wire.begin();                                       //start the I2C
  Serial.begin(9600);                                 //Set the hardware serial port to 9600

  delay(3000);                                        //wait for devices to boot
  print_help();                                       //show instructions
  scan_devices();                                     //find the devices
  Serial.println();
  Seq.reset();                                        //initialize the sequencer
}

void loop() {
  String cmd;                             //variable to hold commands we send to the kit

  if (receive_command(cmd)) {             //if we sent the kit a command it gets put into the cmd variable
    polling = false;                      //we stop polling
    if (!process_coms(cmd)) {             //then we evaluate the cmd for kit specific commands
      process_command(cmd, device_list, device_list_len, default_board);    //then if its not kit specific, pass the cmd to the IOT command processing function
    }
  }

  if (polling == true) {                  //if polling is turned on, run the sequencer
    Seq.run();
  }
}

void change_address(uint8_t address){
  Unknown_Device.send_cmd_with_num("I2C,", address, 0);            //send the command to change the I2C address to the device
  delay(3000);
  Unknown_Device.set_address(address);                             //change the object's address to match the devices new address
}

void step1() {
  //send a read command
  Unknown_Device.send_read_cmd();
}

void step2() {
  receive_and_print_reading(Unknown_Device);             //get the reading from the device
  Serial.println("");
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

      float mintime = reading_delay * 2 + 300;
      if (new_delay >= (mintime / 1000.0)) {                                     //make sure its greater than our minimum time
        Seq.set_step2_time((new_delay * 1000.0) - mintime);          //convert to milliseconds and remove the reading delay from our wait
      } else {
        Serial.println("delay too short");                          //print an error if the polling time isnt valid
      }
    }
    return true;
  }
  
  else if (string_buffer.startsWith("ADDR")) {
    int16_t index = string_buffer.indexOf(',');                    //check if were passing a polling delay parameter
    if (index != -1) { 
      int new_addr = string_buffer.substring(index + 1).toInt();
      change_address(new_addr);
      Serial.println("address changed");
    }
    return true;
  }
  return false;                   //print an error if the polling time isnt valid
}

void print_help() {
  Serial.println(F("Atlas Scientific I2C Scan Demo                                             "));
  Serial.println(F("Commands:                                                                  "));
  Serial.println(F("poll         Takes readings continuously of all sensors                    "));
  Serial.println(F("addr,[nnn]   Changes the sensors I2C address to number nnn                 "));

}
