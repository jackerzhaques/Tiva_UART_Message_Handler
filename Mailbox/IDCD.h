/*
 * Inter-Device Communication Description
 *
 * This file contains a list of all messages described in the Inter-Device Communication Description (IDCD).
 */

#ifndef IDCD_H
#define IDCD_H

#define HEARTBEAT_MESSAGE_ID        1
#define HEARTBEAT_MESSAGE_DLC       0
#define HEARTBEAT_MESSAGE_DATA_INIT  {}
#define HEARTBEAT_MESSAGE_INIT      {HEARTBEAT_MESSAGE_ID, HEARTBEAT_MESSAGE_DLC, HEARTBEAT_MESSAGE_DATA_INIT}

typedef struct HeartbeatMessage_data_tag{
} HeartbeatMessage_data;

typedef struct HeartbeatMessage_tag{
    uint16_t ID;
    uint8_t DLC;
    HeartbeatMessage_data data;
} HeartbeatMessage;

#define HEARTBEAT_MESSAGE_RESPONSE_ID           2
#define HEARTBEAT_MESSAGE_RESPONSE_DLC          0
#define HEARTBEAT_MESSAGE_RESPONSE_DATA_INIT    {}
#define HEARTBEAT_MESSAGE_RESPONSE_INIT         {HEARTBEAT_MESSAGE_RESPONSE_ID, HEARTBEAT_MESSAGE_RESPONSE_DLC, HEARTBEAT_MESSAGE_RESPONSE_DATA_INIT}

typedef struct HeartbeatMessageResponse_data_tag{
} HeartbeatMessageResponse_data;

typedef struct HeartbeatMessageResponse_tag{
    uint16_t ID;
    uint8_t DLC;
} HeartbeatMessageResponse;

#endif
