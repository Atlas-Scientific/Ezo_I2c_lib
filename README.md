# Ezo_I2c_lib
Library for using Atlas Scientific EZO devices in I2C mode

## Instructions
To use the library with Arduino, follow [this link](https://www.arduino.cc/en/Guide/Libraries).

For instructions on how to set up the devices in I2C mode, see [this instructable](https://www.instructables.com/id/UART-AND-I2C-MODE-SWITCHING-FOR-ATLAS-SCIENTIFIC-E/)

See our [instructables page](https://www.instructables.com/member/AtlasScientific/) to learn how to assemble the hardware for some of the examples

## Documentation
```C++
//errors
enum errors {SUCCESS, FAIL, NOT_READY, NO_DATA, NOT_READ_CMD};

//constructors
Ezo_board(uint8_t address);	 //Takes I2C address of the device
Ezo_board(uint8_t address, const char* name); 
//Takes I2C address of the device
//as well a name of your choice

void send_cmd(const char* command);	
//send any command in a string, see the devices datasheet for available i2c commands

void send_read_cmd();	
//sends the "R" command to the device and sets issued_read() to true, 
//so we know to parse the data when we receive it with receive_read()

void send_cmd_with_num(const char* cmd, float num, uint8_t decimal_amount = 3);
//sends any command with the number appended as a string afterwards.
//ex. PH.send_cmd_with_num("T,", 25.0); will send "T,25.000"

void send_read_with_temp_comp(float temperature);
//sends the "RT" command with the temperature converted to a string
//to the device and sets issued_read() to true, 
//so we know to parse the data when we receive it with receive_read()

enum errors receive_cmd(char* sensordata_buffer, uint8_t buffer_len); 
//receive the devices response data into a buffer you supply.
//Buffer should be long enough to hold the longest command 
//you'll receive. We recommand 32 bytes/chars as a default

enum errors receive_read_cmd(); 
//gets the read response from the device, and parses it into the reading variable
//if send_read() wasn't used to send the "R" command and issued_read() isnt set, the function will 
//return the "NOT_READ_CMD" error

bool is_read_poll();		
//function to see if issued_read() was set. 
//Useful for determining if we should call receive_read() (if is_read_poll() returns true) 
//or receive_cmd() if is_read_poll() returns false) 

float get_last_received_reading();		
//returns the last reading the device received as a float

const char* get_name();		
//returns a pointer to the name string

uint8_t get_address();
//returns the address of the device

enum errors get_error();	
//returns the error status of the last received command, 
//used to check the validity of the data returned by get_reading()
```


# Ezo_i2c_util
Common functions used by Atlas Scientific Ezo sample code for printing various responses and info that would otherwise be duplicated in many examples.

```C++
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
```

# iot_cmd.h
Common functions used by Atlas Scientific Ezo sample code to receive user commands and send them to the circuits included in the IOT kits

```C++
bool receive_command(String &string_buffer);
//used to receive commands from the serial port, print them,
//strip them of whitespace, and uppercase them to get them ready for processing
//returns true if a command was received and false if it wasn't

void process_command(const String &string_buffer, Ezo_board device_list[], 
                     uint8_t device_list_len, Ezo_board* default_board);
//handles the common functions of the IOT kit command processing
//such as listing boards, sending commands to one or all boards, and
//switching which board commands are sent to by default
                     
void list_devices(Ezo_board device_list[], uint8_t device_list_len, Ezo_board* default_board);
//prints a list of all the boards in the device list array, with an arrow showing which board is
//the default board
```

# Sequencer# libs
Several classes that allow easy sequencing of events without blocking. Useful for timer based events, running code concurrently, and allowing serial communication in scenarios where long delays would impede it. See the examples for how to work with this library.