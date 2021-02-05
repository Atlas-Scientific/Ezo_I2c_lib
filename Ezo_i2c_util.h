
#ifndef EZO_I2C_UTIL_H
#define EZO_I2C_UTIL_H

#include "Arduino.h"

#include "Ezo_i2c.h"

// these are common functions used by Atlas Scientific Ezo sample code
// for printing various responses and info 
// that would otherwise be duplicated in many examples

void print_device_info(Ezo_board &Device);
//prints the name and address of the given device

void print_success_or_error(Ezo_board &Device, const char* success_string);
//prints different responses depending on whether the device has successfully 
//recieved its message or not
//for the error case, where the device gets an improper command, it prints "Failed "
//for the not ready case, where the device isnt done processing the command, it prints "Pending "
//for the no data case, where it cannot communicate with the device, it prints "No Data "
//if the message is received correctly it prints the success_string that's passed in. This can be used
//to print the reading in cases where the command succeeds for example

void receive_and_print_reading(Ezo_board &Device);
//used to handle receiving readings and printing them in a common format
//typical use case is calling it 1 second after sending the send_read_cmd function from the Ezo_i2c_lib

void receive_and_print_response(Ezo_board &Device);
//used to handle receiving responses and printing them in a common format
//typical use case is calling it after sending commands like send_cmd or send_cmd_with_num 
//from the Ezo_i2c_lib and waiting the appropriate amount of time for the command



#endif