#include <Servo.h>
#include <AFMotor.h>

namespace ArduCarForth {
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
  int stackBottom = MAX_STACK_DEPTH;
  
  struct Word {
    boolean isCompileTime;
    boolean isNative;
    union {
      void (*nativeFunction)();
      Word *forthFunction;
    } implementation;
    char *text;
    
    void run( boolean compileTime ) const {
      if( compileTime && !isCompileTime ) {
        Serial.print("Word '");
        Serial.print(text);
        Serial.println("' is not a compile-time word, so ignoring at compile-time.");
        // TODO: Add to definition
      } else {
        if( isNative ) {
          implementation.nativeFunction();
        } else {
          Serial.print("Word '");
          Serial.print(text);
          Serial.println("' is not native; ignoring for now.");
          // TODO: something with implementation.forthFunction
        }
      }
    }
  };
  
  template <class Item>
  struct Dictionary {
    Dictionary<Item> *previous;
    Item *begin;
    Item *end;
    
    Item *find( char *text ) const {
      for( Item *i = end-1; i>=begin; --i ) {
        if( strcmp(i->text, text) == 0 ) return i;
      }
      return NULL;
    }
  };
  
  void push(int v) {
    if( stackBottom == 0 ) {
      Serial.print("# Error: stack full at ");
      Serial.print(MAX_STACK_DEPTH);
      Serial.print(" items; cannot push value: ");
      Serial.println(v);
    } else {
      stack[--stackBottom] = v;
    }
  }
  
  int pop() {
    if( stackBottom == MAX_STACK_DEPTH ) {
      Serial.println("# Error: stack underflow");
      return 0;
    } else { 
      return stack[stackBottom++];
    }
  }
  
  //// Builtin words ////
  
  void pushStackFree() {
    push( stackBottom );
  }
  void printStack() {
    for( int i = MAX_STACK_DEPTH - 1; i >= stackBottom; --i ) {
      Serial.print( stack[i] );
      Serial.print( " " );
    }
    Serial.println();
  }
  void printIntFromStack() {
    Serial.println( pop() );
  }
  void addIntsFromStack() {
    push( pop() + pop() );
  }
  void subtractIntsFromStack() {
    int b = pop();
    int a = pop();
    push( a - b );
  }
  
  AF_DCMotor motor1(1, MOTOR12_64KHZ);
  AF_DCMotor motor3(3, MOTOR12_64KHZ);
  
  // motor number, speed ->
  void setMotorSpeed() {
    int speed = pop();
    int motorId = pop();
    
    AF_DCMotor *motor;
    if( motorId == 1 ) {
      motor = &motor1;
    } else if( motorId == 3 ) {
      motor = &motor3;
    } else {
      return;
    }
    
    if( speed == 0 ) {
      motor->setSpeed( 0 );
      motor->run( RELEASE );
    } else if( speed < 0 ) {
      motor->setSpeed( -speed );
      motor->run( BACKWARD );
    } else {
      motor->setSpeed( speed );
      motor->run( FORWARD );
    }
  }
  
  const Word staticWords[] = {
    {
      isCompileTime: false,
      isNative: true,
      implementation: {
        nativeFunction: pushStackFree
      },
      text: "stack-free"
    },
    {
      isCompileTime: false,
      isNative: true,
      implementation: {
        nativeFunction: printStack
      },
      text: "print-stack"
    },
    {
      isCompileTime: false,
      isNative: true,
      implementation: {
        nativeFunction: printIntFromStack
      },
      text: "print-int"
    },
    {
      isCompileTime: false,
      isNative: true,
      implementation: {
        nativeFunction: addIntsFromStack
      },
      text: "+"
    },
    {
      isCompileTime: false,
      isNative: true,
      implementation: {
        nativeFunction: subtractIntsFromStack
      },
      text: "-"
    },
    {
      isCompileTime: false,
      isNative: true,
      implementation: {
        nativeFunction: setMotorSpeed
      },
      text: "set-motor-speed"
    }
  };
  
  ////
  
  const Dictionary<const Word> staticDict = {
    previous: NULL,
    begin: staticWords,
    end: staticWords + (int)(sizeof(staticWords)/sizeof(Word))
  };
  
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
      push(intVal);
      return;
    }
    
    const Word *word = staticDict.find(buffer);
    if( word != NULL ) {
      word->run( false );
      return;
    }
    
    Serial.print("# Error: unrecognised word: ");
    Serial.println(tokenBuffer);
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
    ArduCarForth::handleChar(Serial.read());
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

