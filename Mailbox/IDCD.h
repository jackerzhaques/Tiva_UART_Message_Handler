/*
 * Inter-Device Communication Description
 *
 * This file contains a list of all messages described in the Inter-Device Communication Description (IDCD).
 */

#ifndef IDCD_H
#define IDCD_H

#define HEARTBEAT_MESSAGE_ID        1
#define HEARTBEAT_MESSAGE_DLC       2
#define HEARTBEAT_MESSAGE_DATA_INIT  {0, 0, 0, 0}
#define HEARTBEAT_MESSAGE_INIT      {HEARTBEAT_MESSAGE_ID, HEARTBEAT_MESSAGE_DLC, HEARTBEAT_MESSAGE_DATA_INIT}

typedef struct HeartbeatMessage_data_tag{
    uint8_t status;
    uint8_t enabled : 1;
    uint8_t reserved : 7;
    uint8_t remainder[6];
} HeartbeatMessage_data;

typedef struct HeartbeatMessage_tag{
    uint16_t ID;
    uint8_t DLC;
    HeartbeatMessage_data data;
} HeartbeatMessage;

#endif
