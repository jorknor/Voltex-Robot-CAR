#include "MainStateMachine.h"


//array of state function pointers
void (*stateFunctions[6])(struct State*) = {0};

struct State currentState = {
    .id = Idle,
    .currentEvent = None
};

//this is probably an unnecessary abstraction
void registerNewState(enum StateID funcID, void (*newFunc)(struct State*)) {
    stateFunctions[funcID] = newFunc;
}


void runCurrentState() {
    void (*stateFunc)(struct State*) = stateFunctions[currentState.id];
    
    //check if the function pointer is valid
    if (stateFunc != nullptr) {
        stateFunc(&currentState);
    }
}

