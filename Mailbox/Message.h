#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MESSAGE_DATA_LENGTH     16

#define MESSAGE_START_BYTE      ':'
#define MESSAGE_ID_END_BYTE     ' '
#define MESSAGE_END_BYTE        ';'

#include <stdint.h>

typedef struct Message_tag{
    uint16_t ID : 11;
    char data[MAX_MESSAGE_DATA_LENGTH];

    uint8_t reserved : 5;
} Message;

#endif
