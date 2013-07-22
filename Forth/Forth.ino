namespace ArduForth {
  const byte MAX_TOKEN_LENGTH = 32;
  const byte MAX_STACK_DEPTH = 32;
  
  template <class Item>
  struct Pool {
    Item *begin;
    Item *end;
    
    Item *allocate( int count ) {
      if( begin + count <= end ) {
        Item *b = begin;
        begin += count;
        return b;
        return true;
      } else {
        return false;
      }
    };
  };
  
  char tokenBuffer[MAX_TOKEN_LENGTH];
  byte tokenBufferLength = 0;
  
  int stack[MAX_STACK_DEPTH];
  int stackDepth = 0;
  
  void handleWord( char *buffer, int len ) {
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
      stack[stackDepth--] = intVal;
    } else {
      Serial.println("You entered some non-integer");
    }
  }
  
  void flushToken() {
    tokenBuffer[tokenBufferLength] = 0;
    if( tokenBufferLength > 0 ) {
      handleWord( tokenBuffer, tokenBufferLength );
    }
    tokenBufferLength = 0;
  }
  
  void handleChar( char c ) {
    // TODO: Handle double-quoted strings
    switch( c ) {
    case '\n': case ' ': case '\t': case '\r':
      flushToken();
      break;
    default:
      tokenBuffer[tokenBufferLength++] = c;
    }
    if( tokenBufferLength >= MAX_TOKEN_LENGTH ) {
      Serial.println("Token too long.");
    }
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
    ArduForth::handleChar(Serial.read());
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

