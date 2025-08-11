#include <iot_cmd.h>
#include <sequencer2.h>    //imports a 4 function sequencer
#include <sequencer1.h>    //imports a 1 function sequencer
#include <Ezo_i2c_util.h>  //brings in common print statements
#include <Ezo_i2c.h>       //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>          //include arduinos i2c library

const char device_names[10][17] = { 0 }; //arrays for holding the names of the devices

//all the devices we can find at once. Add or remove these to communicate with more circuits or use less resources
Ezo_board Device1 = Ezo_board(0, device_names[0]);  
Ezo_board Device2 = Ezo_board(0, device_names[1]);  
Ezo_board Device3 = Ezo_board(0, device_names[2]);  
Ezo_board Device4 = Ezo_board(0, device_names[3]);  
Ezo_board Device5 = Ezo_board(0, device_names[4]);  
Ezo_board Device6 = Ezo_board(0, device_names[5]);  
Ezo_board Device7 = Ezo_board(0, device_names[6]);  
Ezo_board Device8 = Ezo_board(0, device_names[7]);  
Ezo_board Device9 = Ezo_board(0, device_names[8]);  
Ezo_board Device10 = Ezo_board(0, device_names[9]); 


Ezo_board device_list[] = {  //an array of boards used for sending commands to all or specific boards
  Device1,
  Device2,
  Device3,
  Device4,
  Device5,
  Device6,
  Device7,
  Device8,
  Device9,
  Device10
};

Ezo_board *default_board = &device_list[0];  //used to store the board were talking to

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

const unsigned long reading_delay = 1000;  //how long we wait to receive a response, in milliseconds

unsigned int poll_delay = 100;  //how long to wait between polls after accounting for the times it takes to send readings

int found_devices = 0;

bool polling = true;  //variable to determine whether or not were polling the circuits

void step1();  //forward declarations of functions to use them in the sequencer before defining them
void step2();
Sequencer2 Seq(&step1, reading_delay,  //calls the steps in sequence with time in between them
               &step2, poll_delay);

void receive_reading(Ezo_board &Device) {              // function to decode the reading after the read command was issued
  Serial.print(Device.get_name()); Serial.print(" ");  // print the name of the circuit getting the reading
  Serial.print(Device.get_address()); Serial.print(": ");
  Device.receive_read_cmd();              //get the response data and put it into the [Device].reading variable if successful
  print_success_or_error(Device, String(Device.get_last_received_reading(), 2).c_str());  //print either the reading or an error message
}

void receive_devicetype(Ezo_board &Device) {
  char receive_buffer[32];                 //buffer used to hold each boards response
  Device.receive_cmd(receive_buffer, 32);  //put the response into the buffer
  Serial.println(receive_buffer);

  int length = 0;
  for(length = 0; length < 10; length++){
    if(receive_buffer[3 + length] == ',') { break; }
  }
  if (Device.get_error() == Ezo_board::SUCCESS) {
    memset(Device.get_name(), 0, 17);
    strncpy(Device.get_name(), &receive_buffer[3], length);
  } else {
    memset(Device.get_name(), 0, 17);
    strcpy(Device.get_name(), "Unknown");
  }
}

void receive_name(Ezo_board &Device) {
  char receive_buffer[32];                 //buffer used to hold each boards response
  Device.receive_cmd(receive_buffer, 32);  //put the response into the buffer
  // Serial.println(receive_buffer);

  int length = 0;
  for(length = 0; length < 16; length++){
    if(receive_buffer[6 + length] == 0) { break; }
  }
  if (Device.get_error() == Ezo_board::SUCCESS) {
    if(length > 1){ //only overwrite if we have a name
      memset(Device.get_name(), 0, 17);
      strncpy(Device.get_name(), &receive_buffer[6], length);
    }
  } else {
    strcpy(Device.get_name(), "Name Error");
  }
}

bool scan_devices() {
  found_devices = 0;
  for (uint8_t i = 20; i < 128; ++i) {            //step through the i2C addresses
    Wire.beginTransmission(i);                    //try to talk to the device over I2C
    uint8_t error = Wire.endTransmission();       //check if communication attempt was successful
    if (error == 0) {                             //if its possible to communicate with it
      device_list[found_devices].set_address(i);  //set that to be the address
      device_list[found_devices].send_cmd("I");  //ask what kind of device it is
      if (found_devices < device_list_len) {
        found_devices++;
      }
    }
  }
  return (found_devices > 0);
}

void receive_devicetypes(){
  delay(400);
  for (int i = 0; i < found_devices; i++) {
    receive_devicetype(device_list[i]);
  }
}

void query_name(){
  for (int i = 0; i < found_devices; i++) {
    device_list[i].send_cmd("name,?");
  }
  delay(400);
  for (int i = 0; i < found_devices; i++) {
    receive_name(device_list[i]);
  }
}

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

  Wire.begin();        //start the I2C
  Serial.begin(9600);  //start the serial communication to the computer
  delay(1000); //if arduino was restarted but the devices are still reading we won't get their credentials
  if (!scan_devices()) { Serial.println("No EZO devices found!"); }
  receive_devicetypes();
  query_name();
  Seq.reset();
}

void loop() {
  String cmd;  //variable to hold commands we send to the kit

  if (receive_command(cmd)) {                                             //if we sent the kit a command it gets put into the cmd variable
    polling = false;                                                      //we stop polling
    if (!process_coms(cmd)) {                                             //then we evaluate the cmd for kit specific commands
      process_command(cmd, device_list, found_devices, default_board);  //then if its not kit specific, pass the cmd to the IOT command processing function
    }
  }

  if (polling == true) {  //if polling is turned on, run the sequencer
    Seq.run();
  }
}

void step1() {
  //send a read command to all devices
  //to let the library know to parse the reading
  for (int i = 0; i < found_devices; i++) {
    device_list[i].send_read_cmd();
  }
}

void step2() {
  for (int i = 0; i < found_devices; i++) {
    receive_reading(device_list[i]);  //get the reading from the PH circuit
    Serial.println();
  }
  Serial.println("----------");
}

bool process_coms(const String &string_buffer) {  //function to process commands that manipulate global variables and are specifc to certain kits
  if (string_buffer == "HELP") {
    print_help();
    return true;
  } else if (string_buffer == "RESCAN") {
    delay(1000); //wait for the readings to finish
    for (int i = 0; i < found_devices; i++) {
      device_list[i].receive_read_cmd();
    }
    if (!scan_devices()) { Serial.println("No EZO devices found!"); }
    receive_devicetypes();
    query_name();
    Seq.reset();
    Serial.println("Done! Enter poll to restart polling");
    return true;
  }else if (string_buffer.startsWith("POLL")) {
    polling = true;
    Seq.reset();

    int16_t index = string_buffer.indexOf(',');                        //check if were passing a polling delay parameter
    if (index != -1) {                                                 //if there is a polling delay
      float new_delay = string_buffer.substring(index + 1).toFloat();  //turn it into a float

      float mintime = reading_delay * 2 + 300;
      if (new_delay >= (mintime / 1000.0)) {                 //make sure its greater than our minimum time
        Seq.set_step2_time((new_delay * 1000.0) - mintime);  //convert to milliseconds and remove the reading delay from our wait
      } else {
        Serial.println("delay too short");  //print an error if the polling time isnt valid
      }
    }
    return true;
  }
  return false;  //return false if the command is not in the list, so we can scan the other list or pass it to the circuit
}


void print_help() {

  Serial.println(F("Atlas Scientific I2C Communications Sample Code                            "));
  Serial.println(F("Commands:                                                                  "));
  Serial.println(F("poll         Takes readings continuously of all sensors                    "));
  Serial.println(F("                                                                           "));
  Serial.println(F("rescan       adds new devices and removes old devices if they're inactive  "));

  Serial.println();
  iot_cmd_print_listcmd_help();
  Serial.println();
  iot_cmd_print_allcmd_help();
}
