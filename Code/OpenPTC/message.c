#include "message.h"
#include "xbee.h"

uint8_t *msgPtr;
uint8_t msgType;

void processDirectedMessage()
{

    msgPtr = getRX();
    msgType = msgPtr[0];           // get our message code

    switch(msgType)
    {
        case 0:
               break;
    }
}
