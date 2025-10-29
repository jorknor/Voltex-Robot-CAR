#pragma once
#include "Drivers/BluetoothDriver.h"
#include "Drivers/SerialDriver.h"
#include "MainStateMachine.h"

#define INPUT_WORD_SIZE 8
#define INPUT_MAX_ARGS 2
extern char inputBuffer[INPUT_MAX_ARGS][INPUT_WORD_SIZE] = {0};
extern unsigned char argumentCount = 0;
void flushArguments();
int strncmp( const char * s1, const char * s2, unsigned int n );
void inputManagerUpdate();
