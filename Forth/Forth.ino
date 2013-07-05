const byte ArduForth_MAX_TOKEN_LENGTH = 32;

char ArduForth_tokenBuffer[ArduForth_MAX_TOKEN_LENGTH];
byte ArduForth_tokenBufferLength = 0;

void ArduForth_flushToken() {
  ArduForth_tokenBuffer[ArduForth_tokenBufferLength] = 0;
  if( ArduForth_tokenBufferLength > 0 ) {
    Serial.print("You said: ");
    Serial.println(ArduForth_tokenBuffer);
  }
  ArduForth_tokenBufferLength = 0;
}

void ArduForth_handleChar( char c ) {
  // TODO: Handle double-quoted strings
  switch( c ) {
  case '\n': case ' ': case '\t': case '\r':
    ArduForth_flushToken();
    break;
  default:
    ArduForth_tokenBuffer[ArduForth_tokenBufferLength++] = c;
  }
  if( ArduForth_tokenBufferLength >= ArduForth_MAX_TOKEN_LENGTH ) {
    Serial.println("Token too long.");
  }
}

typedef unsigned long int millis_t;

void setup() {
  Serial.begin(9600);
  while( !Serial ) { }
  Serial.println();
  Serial.println("Hello, world!");
}

millis_t nextTickTime = 0;
void loop() {
  millis_t time = millis();
  while( Serial.available() > 0 ) {
    ArduForth_handleChar(Serial.read());
  }
  
  
  /*  timer demonstration
  if( time - nextTickTime >= 0 ) {
    if( Serial ) {
      Serial.print("Tick! Target: ");
      Serial.print(nextTickTime);
      Serial.print(", actual:");
      Serial.print(time);
      Serial.println();
    }
    nextTickTime += 1000;
    // Compare by substraction to avoid weird behavior
    // near overflow
    if( time - nextTickTime > 0 ) {
      // Still behind?
      nextTickTime = time + 1000;
    }
  }
  */
}

