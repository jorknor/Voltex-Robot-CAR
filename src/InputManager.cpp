#include "InputManager.h"


int strncmp( const char * s1, const char * s2, unsigned int n )
{
    while ( n && *s1 && ( *s1 == *s2 ) )
    {
        ++s1;
        ++s2;
        --n;
    }
    if ( n == 0 )
    {
        return 0;
    }
    else
    {
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
    }
}


char inputBuffer[INPUT_MAX_ARGS][INPUT_WORD_SIZE] = {0};
unsigned char argumentCount = 0;


void parseArguments(char* data, int size) {
    unsigned char argumentBase = 0;

    for (int i = 0; i < size; i++) {
        if (data[i] == ' ') {
            if (argumentCount++ > INPUT_MAX_ARGS) {
                break;
            }
            argumentBase = 0;
        }
        inputBuffer[argumentCount][argumentBase] = data[i];
        if (argumentBase++ > INPUT_WORD_SIZE) {
            if (argumentCount++ > INPUT_MAX_ARGS) {
                break;
            }
            argumentBase = 0;
        }
    }
}

void flushArguments() {
    argumentCount = 0;
    currentState.currentEvent = None;
}


void inputManagerUpdate() {
    //make sure that the input is handeled
    if (currentState.currentEvent == InputComplete) {
        return;
    }

    if (serialInputComplete) {
        parseArguments(serialBuffer, serialInputSize);
        serialInputSize = 0;
        serialInputComplete = false;
        currentState.currentEvent = InputComplete;
    } else if (bluetoothStringComplete) {
        parseArguments(bluetoothInputString, bluetoothInputSize);
        
        bluetoothStringComplete = false;
        bluetoothInputSize = 0;
        
    }

    if (strncmp(inputBuffer[0], "setStateIdle", INPUT_WORD_SIZE) == 0) {
        Serial.print("changed state to idle\r\n");
        currentState.id = Idle;
        flushArguments();
    }

}

