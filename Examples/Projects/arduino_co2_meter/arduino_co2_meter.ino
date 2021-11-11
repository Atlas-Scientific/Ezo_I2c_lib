

//This code is for the "Arduino CO2 Meter" project.
//The CO2 Meter uses an Atlas Scientific Co2 sensor connected to an Arduino Uno and I2C LCD display.

//Project link: https://www.hackster.io/atlasscientific/arduino-co2-meter-c030ce

#include <Ezo_i2c.h>                          //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <sequencer2.h>                       //imports a 2 function sequencer. This controls the I2C read/write timing without blocking other operations    
#include <Ezo_i2c_util.h>                     //brings in common print statements
#include "Adafruit_LiquidCrystal.h"           //include the adafruit i2c lcd backpack library

Ezo_board CO2 = Ezo_board(105, "CO2");        //create a CO2 circuit object, whose address is 105 and name is "CO2"
Adafruit_LiquidCrystal CO2_lcd(0);            //sets the i2c address of the LCD to 0 and names it "CO2_lcd"

void step1();                                 //declare the 2 functions used in the sequencer
void step2();                                 //step 1 sends a read command, step 2 receives the reading

Sequencer2 Seq(&step1, 1000, &step2, 0);      //calls the steps in sequence and sets the time between them

void setup() {
  Serial.begin(9600);                         //enables the serial port and sets the baud rate to 9600
  Seq.reset();                                //initialize the sequencer
  CO2_lcd.begin(16, 2);                       //initialize the 16x2 lcd
  CO2_lcd.setBacklight(HIGH);                 //turn on backlight of lcd
  CO2_lcd.setCursor(1, 0);                    //set cursor to column 1, row 0
  CO2_lcd.print("Carbon Dioxide");            //sends the words "Carbon Dioxide" to line 1 of the display
  CO2_lcd.setCursor(10, 1);                   //set cursor to column 10, row 1
  CO2_lcd.print("ppm");                       //send the words "ppm" to the display
}

void loop() {
  Seq.run();                                  //run the sequncer to get the co2 readings
}

void step1() {
  CO2.send_read_cmd();                        //sends a read command using the Ezo_i2c library
}

void step2() {
  receive_and_print_reading(CO2);             //get the reading from the CO2 sensor
  Serial.println();                           //and send it out through the serial monitor (this is for debugging, you do not need to do this)
  lcd_print();                                //send the Co2 reading to the LCD
}

void lcd_print() {                                    //prints the co2 reading to the LCD

  CO2_lcd.setCursor(3, 1);                            //moves the cursor to column 3, row 1
  CO2_lcd.print("     ");                             //clears the CO2 readings on the display after each iteration 
  CO2_lcd.setCursor(3, 1);                            //moves the cursor to column 3, row 1
  CO2_lcd.print(CO2.get_last_received_reading(), 0);  //sends the CO2 reading to the display
}
