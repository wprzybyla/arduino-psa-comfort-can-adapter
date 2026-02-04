#pragma once

// Include board-specific configuration
#include <BoardConfig_t2can.h>

// Use pins from BoardConfig_t2can.h
#define CS_PIN_CAN0 BOARD_CAN1_CS_PIN  // CAN1 (destination) - pierwszy MCP2515
#define CS_PIN_CAN1 BOARD_CAN2_CS_PIN  // CAN2 (source) - drugi MCP2515
#define SERIAL_SPEED 115200
#define CAN_SPEED CAN_125KBPS // Entertainment CAN bus - Low speed
#define CAN_FREQ MCP_16MHZ // Switch to 8MHZ if you have a 8Mhz module
