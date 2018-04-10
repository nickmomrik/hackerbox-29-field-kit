
// Use pulldown resistors
int btnPins[] = {
  10, 16, 14, 15
};
int btnPinCount = 4;

int ledPins[] = {
  9, 8, 7, 6, 5, 4
};
int ledPinCount = 6;

void setup() {
  for ( int i = 0; i < btnPinCount; i++ ) {
    pinMode( btnPins[ i ], INPUT );
  }

  for ( int i = 0; i < ledPinCount; i++ ) {
    pinMode( ledPins[ i ], OUTPUT );
    digitalWrite( ledPins[ i ], LOW );
  }
}

void loop() {
  for ( int i = 0; i < btnPinCount; i++ ) {
    if ( HIGH == digitalRead( btnPins[ i ] ) ) {
      digitalWrite( ledPins[ i ], HIGH );

      // Wait for button release
      while ( HIGH == digitalRead( btnPins[ i ] ) );

      digitalWrite( ledPins[ i ], LOW );
    }
  }
}
