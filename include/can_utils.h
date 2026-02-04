#pragma once

#include <Arduino.h>
#include <mcp2515.h>

// Function declarations
byte checksumm_0E6(const byte* frame);
void sendPOPup(bool present, int id, byte priority, byte parameters);
int daysSinceYearStartFct();

// External variables needed by these functions
extern struct can_frame canMsgSnd;
extern MCP2515 CAN1;
extern bool SerialEnabled;
extern int alertsCache[];
extern byte alertsParametersCache[];
