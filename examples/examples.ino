#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Pin 21 is kind of bogus here since this OLED doesn't have a RESET pin
Adafruit_SSD1306 display( 21 );

// Button Pins (Use pullup resistors - 10k works well)
int btnPins[] = {
  10, 16, 14, 15
};
int btnPinCount = 4;

// LED Pins
int ledPins[] = {
  9, 8, 7, 6, 5, 4
};
int ledPinCount = 6;

// Variables used for writing to EEPROM
char data[] = "---------------------";
int dataIndex = 0;
char writeVal = 'A';

void setup() {
  // Initialize the button pins
  for ( int i = 0; i < btnPinCount; i++ ) {
    pinMode( btnPins[ i ], INPUT );
  }

  // Initialize the LED pins
  for ( int i = 0; i < ledPinCount; i++ ) {
    pinMode( ledPins[ i ], OUTPUT );
    digitalWrite( ledPins[ i ], LOW );
  }

  // Initialize the OLED
  display.begin( SSD1306_SWITCHCAPVCC, 0x3C );
  display.clearDisplay();
  display.setTextSize( 1 );
  display.setTextColor( WHITE );

  // Initialize for EEPROM read/write
  Wire.begin();

  // Display instructions and previous value from EEPROM
  update_display();
}

void loop() {
  // Check for button presses
  for ( int i = 0; i < btnPinCount; i++ ) {
    if ( LOW == digitalRead( btnPins[ i ] ) ) {
      // Turn on the corresponding LED
      digitalWrite( ledPins[ i ], HIGH );

      // Wait for button to be released
      while ( LOW == digitalRead( btnPins[ i ] ) );

      // Do stuff depending on which button had been pressed
      switch ( i ) {
        case 0:
          // Write to the character array and then EEPROM
          data[ dataIndex ] = writeVal;
          i2c_eeprom_write_page( 0x50, 0, (byte *) data, sizeof( data ) );

          // Advance the index or loop back around to the beginning
          if ( dataIndex < ( sizeof( data ) - 2 ) ) {
            dataIndex++;
          } else {
            dataIndex = 0;
          }
          break;
        case 1:
          update_display();
          break;
        case 2:
          // Next ASCII character
          if ( int( writeVal ) < 126 ) {
            writeVal = char( writeVal + 1 );
          }

          break;
        case 3:
          // Previous ASCII character
          if ( int( writeVal ) > 32 ) {
            writeVal = char( writeVal - 1 );
          }
          break;
      }

      // Turn off the LED
      digitalWrite( ledPins[ i ], LOW );
    }
  }
}

void update_display() {
  // Print instructions
  display.clearDisplay();
  display.setCursor( 0, 0 );
  display.println( "Buttons:" );
  display.println( "1) Save   2) Display" );
  display.println( "3) + Char 4) - Char" );

  // Read characters from EEPROM and print to the display
  int addr = 0;
  byte b = i2c_eeprom_read_byte( 0x50, 0 );
  while ( b != 0 && addr < sizeof( data ) ) {
    display.write( b );

    addr++;
    b = i2c_eeprom_read_byte( 0x50, addr );
  }

  display.display();
}

/*
 * EEPROM code from
 * http://playground.arduino.cc/code/I2CEEPROM
 */

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
    int rdata = data;
    Wire.beginTransmission( deviceaddress );
    Wire.write( (int)( eeaddress >> 8 ) ); // MSB
    Wire.write( (int)( eeaddress & 0xFF ) ); // LSB
    Wire.write( rdata );
    Wire.endTransmission();
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) {
    Wire.beginTransmission( deviceaddress );
    Wire.write( (int)( eeaddresspage >> 8 ) ); // MSB
    Wire.write( (int)( eeaddresspage & 0xFF ) ); // LSB
    byte c;
    for ( c = 0; c < length; c++ ) {
      Wire.write( data[c] );
    }
    Wire.endTransmission();
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
    byte rdata = 0xFF;
    Wire.beginTransmission( deviceaddress );
    Wire.write( (int)( eeaddress >> 8 ) ); // MSB
    Wire.write( (int)( eeaddress & 0xFF ) ); // LSB
    Wire.endTransmission();
    Wire.requestFrom( deviceaddress, 1 );
    if ( Wire.available() ) {
      rdata = Wire.read();
    }

    return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length ) {
    Wire.beginTransmission( deviceaddress) ;
    Wire.write( (int)( eeaddress >> 8 ) ); // MSB
    Wire.write( (int)( eeaddress & 0xFF ) ); // LSB
    Wire.endTransmission();
    Wire.requestFrom( deviceaddress, length );
    int c = 0;
    for ( c = 0; c < length; c++ ) {
        if ( Wire.available() ) {
          buffer[c] = Wire.read();
        }
    }
}
