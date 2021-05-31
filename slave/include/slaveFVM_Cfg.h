#ifndef _SLAVEFVM_TYPES_H
#define _SLAVEFVM_TYPES_H


#include "slaveFVM_Types.h"

#define NUM_RESET 2
#define NUM_ECU 2
#define NUM_MSG 2

#define SYN_ERROR_SIZE 3

uint8 preTrip[3 * NUM_MSG];

uint8 resetData[3 * NUM_RESET];
uint8 preresetData[3 * NUM_MSG];
uint8 msgData[6 * NUM_MSG];
uint8 preMsgData[6 * NUM_MSG];

ResetCntS_Type resetCnt[NUM_RESET];
MsgCntS_Type msgCnt[NUM_MSG];
ResetState_Type resetState[NUM_RESET];

#endif