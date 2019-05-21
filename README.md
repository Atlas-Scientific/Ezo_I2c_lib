# Ezo_I2c_lib
Library for using Atlas Scientific EZO circuits in I2C mode

## Instructions
To use the library with Arduino, follow [this link](https://www.arduino.cc/en/Guide/Libraries).

For instructions on how to set up the circuits in I2C mode, see [this instructable](https://www.instructables.com/id/UART-AND-I2C-MODE-SWITCHING-FOR-ATLAS-SCIENTIFIC-E/)

See our [instructables page](https://www.instructables.com/member/AtlasScientific/) to learn how to assemble the hardware for some of the examples

## Documentation
```C++
//errors
enum errors {SUCCESS, FAIL, NOT_READY, NO_DATA, NOT_READ_CMD};

//constructors
Ezo_board(uint8_t address);	 //Takes I2C address of the sensor
Ezo_board(uint8_t address, const char* name); 
//Takes I2C address of the sensor
//as well a name of your choice

void send_cmd(const char* command);	
//send any command in a string, see the sensors datasheet for available i2c commands

void send_read();	
//sends the "R" command to the sensor and sets issued_read() to true, 
//so we know to parse the data when we receive it with receive_read()

void send_cmd_with_num(const char* cmd, float num, uint8_t decimal_amount = 3);
//sends any command with the number appended as a string afterwards.
//ex. PH.send_cmd_with_num("T,", 25.0); will send "T,25.000"

void send_read_with_temp_comp(float temperature);
//sends the "RT" command with the temperature converted to a string
//to the sensor and sets issued_read() to true, 
//so we know to parse the data when we receive it with receive_read()

enum errors receive_cmd(char* sensordata_buffer, uint8_t buffer_len); 
//receive the sensors response data into a buffer you supply.
//Buffer should be long enough to hold the longest command 
//you'll receive. We recommand 32 bytes/chars as a default

enum errors receive_read(); 
//gets the read response from the sensor, and parses it into the reading variable
//if send_read() wasn't used to send the "R" command and issued_read() isnt set, the function will 
//return the "NOT_READ_CMD" error

bool is_read_poll();		
//function to see if issued_read() was set. 
//Useful for determining if we should call receive_read() (if is_read_poll() returns true) 
//or recieve_cmd() if is_read_poll() returns false) 

float get_reading();		
//returns the last reading the sensor received as a float

const char* get_name();		
//returns a pointer to the name string

enum errors get_error();	
//returns the error status of the last received command, 
//used to check the validity of the data returned by get_reading()
```
