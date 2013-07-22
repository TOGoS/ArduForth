const byte ArduForth_MAX_TOKEN_LENGTH = 32;

char ArduForth_tokenBuffer[ArduForth_MAX_TOKEN_LENGTH];
byte ArduForth_tokenBufferLength = 0;

int ArduForth_stack[32];
int ArduForth_stackDepth = 0;

void ArduForth_handleWord( char *buffer, int len ) {
  boolean isInteger = true;
  int intVal = 0;
  for( int i=0; i<len; ++i ) {
    if( buffer[i] < '0' || buffer[i] > '9' ) {
      isInteger = false;
      break;
    }
    intVal *= 10;
    intVal += buffer[i] - '0';
  }
  
  if( isInteger ) {
    Serial.print("You entered integer: ");
    Serial.println(intVal);
    ArduForth_stack[ArduForth_stackDepth--] = intVal;
  } else {
    Serial.println("You entered some non-integer");
  }
}

void ArduForth_flushToken() {
  ArduForth_tokenBuffer[ArduForth_tokenBufferLength] = 0;
  if( ArduForth_tokenBufferLength > 0 ) {
    ArduForth_handleWord( ArduForth_tokenBuffer, ArduForth_tokenBufferLength );
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
  Serial.println("# Hello, world!");
  Serial.print("# sizeof(int) = ");
  Serial.println(sizeof(int));
  Serial.print("# sizeof(void *) = ");
  Serial.println(sizeof(void *));
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

