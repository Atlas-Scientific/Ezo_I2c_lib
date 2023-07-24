
#include "iot_cmd.h"
#include <Ezo_i2c_util.h>

bool receive_command(String &string_buffer){
    if (Serial.available()) {                           //if theres any characters in the UART buffer
     
        string_buffer = Serial.readString();             //get them until its a complete command
        Serial.print("> ");                               //print whatever we received
        Serial.println(string_buffer);
        string_buffer.toUpperCase();                     //turn the command to uppercase for easier comparisions
        string_buffer.trim();                            //remove all extra spaces and newlines
        return true;
    }
    return false;
}

// determines how long we wait depending on the command
void select_delay(const String &str) {
  if (str.indexOf("CAL") != -1 || str.indexOf("R") != -1) {
    delay(1200);
  } else {
    delay(300);
  }
}

void process_command(const String &string_buffer, Ezo_board device_list[], uint8_t device_list_len, Ezo_board* &default_board){
	Ezo_board* device_list_ptrs[32]; //making this arbitrarily 32 sensors long
	if(device_list_len > 32) {
		return;
	}
	for (uint8_t i = 0; i < device_list_len; i++){
		device_list_ptrs[i] = &device_list[i];
	}	
	process_command(string_buffer, device_list_ptrs, device_list_len, default_board);
}


void process_command(const String &string_buffer, Ezo_board* device_list[], uint8_t device_list_len, Ezo_board* &default_board){
    
  if (string_buffer == "LIST") {                    //if our command is list
    list_devices(device_list, device_list_len, default_board);
  }

  //the all command sends a command to all available boards and shows their responses
  else if (string_buffer.startsWith("ALL:")) {                               //if the command starts with ALL:
    String cmd = string_buffer.substring(string_buffer.indexOf(':') + 1);   //get the rest of the command after the : character

    for (uint8_t i = 0; i < device_list_len; i++) {                            //then send it to every board
      device_list[i]->send_cmd(cmd.c_str());
    }

    select_delay(cmd);                                                        //wait for some time depending on the command

    for (uint8_t i = 0; i < device_list_len; i++) {                            //go through our list of boards and get their response
      receive_and_print_response(*device_list[i]);
    }
  }
  // all other commands are passed through to the default board unless they have an address prepended
  else if (string_buffer != "" ) {                                 //if we received any other commands
    int16_t index = string_buffer.indexOf(':');                    //check if the command contains a colon character, which is used for changing addresses

    if (index != -1) {                                              //if it contains a colon character
      bool addr_found = false;
      String name_to_find = string_buffer.substring(0, index);              //get the address out of the command
      name_to_find.toUpperCase();
      if (name_to_find.length() != 0) {                                              //if its valid
        //search through list and make device match the address
        for (uint8_t i = 0; i < device_list_len; i++) {
          if (name_to_find == device_list[i]->get_name()) {               //if the address matches one of the boards in the list
            default_board = device_list[i];                        //set that board as the default
            addr_found = true;                                      //indicate we changed the address
            break;                                                  //and exit the loop
          }
        }
        if (addr_found) {                                           //then send the rest of the command to that board
          default_board->send_cmd(string_buffer.substring(index + 1).c_str());
        } else {                                                    //otherwise print that we didnt find
          Serial.print("No device named ");
          Serial.println(name_to_find);
          return;
        }
      } else {
        Serial.println("Invalid name");
      }
    }
    else {                                                          //if theres no colon just pass the command to the default board
      default_board->send_cmd(string_buffer.c_str());
    }
	if(string_buffer != "SLEEP"){
		select_delay(string_buffer);                                   //wait for some time depending on the command

		receive_and_print_response(*default_board);
	}
  }
}

void list_devices(Ezo_board device_list[], uint8_t device_list_len, Ezo_board* default_board) {
	
	for (uint8_t i = 0; i < device_list_len; i++) {        //go thorugh the list of boards
		if (default_board == &device_list[i]) {              //if its our default board
		  Serial.print("--> ");                             //print the pointer arrow
		} else {                                            //otherwise
		  Serial.print(" - ");                              //print a normal dash
		}
		print_device_info(device_list[i]);                   //then print the boards info
		Serial.println("");
	}
  
}
	

void list_devices(Ezo_board* device_list[], uint8_t device_list_len, Ezo_board* default_board) {
  for (uint8_t i = 0; i < device_list_len; i++) {        //go thorugh the list of boards
    if (default_board == device_list[i]) {              //if its our default board
      Serial.print("--> ");                             //print the pointer arrow
    } else {                                            //otherwise
      Serial.print(" - ");                              //print a normal dash
    }
    print_device_info(*device_list[i]);                   //then print the boards info
    Serial.println("");
  }
}


void iot_cmd_print_listcmd_help(){
  //Serial.println(F("                                                                           "));
	Serial.println(F("list              prints the names and addresses of all the devices in the "));
	Serial.println(F("                  device list, with an arrow pointing to the default board "));
}

void iot_cmd_print_allcmd_help(){
	Serial.println(F("all:[query]       sends a query to all devices in the device list, and     "));
	Serial.println(F("                  prints their responses"));
}

void iot_cmd_print_namedquery_help(){
	Serial.println(F("[name]:[query]    sends a query to the device with the name [name], and "));
	Serial.println(F("                  makes that device the default board"));
	Serial.println(F("                      ex: PH:status sends a status query to the device "));
	Serial.println(F("                      named PH"));
}