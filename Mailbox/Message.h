#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MESSAGE_DATA_LENGTH     8  //Bytes
#define MAX_MESSAGE_ASCII_LENGTH    31 //8 bytes in ASCII representation + 7 commas

#define MESSAGE_START_BYTE          ':'
#define MESSAGE_ID_END_BYTE         ' '
#define MESSAGE_END_BYTE            ';'
#define MESSAGE_FRAMING_BYTE        ','     //Separates ascii data bytes

#include <stdint.h>

typedef struct Message_tag{
    uint16_t ID : 11;
    uint8_t data[MAX_MESSAGE_DATA_LENGTH];

    uint8_t reserved : 5;
} Message;

#endif
