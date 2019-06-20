#include "Mailbox.h"
#include "UARTHandler.h"

#include <stdio.h>
#include <string.h>
#include <driverlib/interrupt.h>

static MessageQueue IncomingMessages;

//Function prototypes

void InitializeMailbox(void){
    InitializeMessageQueue(&IncomingMessages);
    InitializeUARTHandler();
}

void SendMessage(Message m){
    char end[1];
    end[0] = MESSAGE_END_BYTE;

    //Maximum message ID is 4 characters long
    char header[7];
    snprintf(header, 6, "%c%u%c", MESSAGE_START_BYTE, m.ID, MESSAGE_ID_END_BYTE);

    //Disable interrupts while adding bytes to buffer
    IntMasterDisable();

    AddBytesToBuffer(header, strlen(header));
    AddBytesToBuffer(m.data, strlen(m.data));
    AddBytesToBuffer(end, 1);

    IntMasterEnable();
}

Message* GetNextReceivedMessage(void){
    return GetNextMessage(&IncomingMessages);
}
