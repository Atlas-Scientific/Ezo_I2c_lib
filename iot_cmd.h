
#ifndef IOT_CMD_H
#define IOT_CMD_H

#include "Arduino.h"

#include <Ezo_i2c.h>


bool receive_command(String &string_buffer);
//used to receive commands from the serial port, print them,
//strip them of whitespace, and uppercase them to get them ready for processing
//returns true if a command was received and false if it wasn't

void process_command(const String &string_buffer, Ezo_board device_list[], 
                     uint8_t device_list_len, Ezo_board* &default_board);
//handles the common functions of the IOT kit command processing
//such as listing boards, sending commands to one or all boards, and
//switching which board commands are sent to by default
                     
void list_devices(Ezo_board device_list[], uint8_t device_list_len, Ezo_board* default_board);
//prints a list of all the boards in the device list array, with an arrow showing which board is
//the default board
#endif