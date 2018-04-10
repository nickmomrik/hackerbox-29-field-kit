#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Bogus 21 pin since the OLED I'm using doesn't have a RESET pin
Adafruit_SSD1306 display( 21 );

// Use pulldown resistors
int btnPins[] = {
  10, 16, 14, 15
};
int btnPinCount = 2; // Set to 4 if using all of them

int ledPins[] = {
  9, 8, 7, 6, 5, 4
};
int ledPinCount = 2; // Set to 6 if you want to use all of them

char data[] = "0000000000";
int dataIndex = 0;

void setup() {
  for ( int i = 0; i < btnPinCount; i++ ) {
    pinMode( btnPins[ i ], INPUT );
  }

  for ( int i = 0; i < ledPinCount; i++ ) {
    pinMode( ledPins[ i ], OUTPUT );
    digitalWrite( ledPins[ i ], LOW );
  }

  display.begin( SSD1306_SWITCHCAPVCC, 0x3C );
  display.clearDisplay();
  display.setTextSize( 1 );
  display.setTextColor( WHITE );
  display.display();

  Wire.begin();
}

void loop() {
  for ( int i = 0; i < btnPinCount; i++ ) {
    if ( HIGH == digitalRead( btnPins[ i ] ) ) {
      digitalWrite( ledPins[ i ], HIGH );

      display.clearDisplay();
      display.setCursor( 0, 0 );

      // Wait for button release
      while ( HIGH == digitalRead( btnPins[ i ] ) );

      if ( 0 == i ) {
        if ( dataIndex < sizeof( data ) - 1 ) {
          data[ dataIndex ] = '1';
          dataIndex++;
          i2c_eeprom_write_page( 0x50, 0, (byte *) data, sizeof( data ) );
        }
      } else if ( 1 == i ) {
        int addr = 0; //first address
        byte b = i2c_eeprom_read_byte( 0x50, 0 ); // access the first address from the memory

        while ( b != 0 ) {
          display.write( b );
          display.display();

          addr++;
          b = i2c_eeprom_read_byte( 0x50, addr );
        }
      }

      digitalWrite( ledPins[ i ], LOW );
    }
  }
}

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
