#pragma once

#include <cstdio>

/**
 * @file config.h
 * @brief Project configuration and pin mappings
 * 
 * This file centralizes all project configuration including:
 * - Pin definitions (from board-specific config)
 * - CAN bus settings
 * - Serial communication settings
 */

// Include board-specific pin definitions
#include <BoardConfig_t2can.h>

// CAN Controller Pin Mappings
// CAN0: Connected to vehicle's CAN2004 bus (destination)
// CAN1: Connected to CAN2010 device like NAC/SMEG (source)
#define CS_PIN_CAN0 BOARD_CAN1_CS_PIN  // CAN0 chip select pin
#define CS_PIN_CAN1 BOARD_CAN2_CS_PIN  // CAN1 chip select pin

// Serial Communication
#define SERIAL_SPEED 115200  // Baud rate for Serial monitor

// CAN Bus Configuration
#define CAN_SPEED CAN_125KBPS  // Entertainment CAN bus speed (125 kbps)
#define CAN_FREQ MCP_16MHZ     // MCP2515 oscillator frequency (16 MHz)
                              // Change to MCP_8MHZ if using 8 MHz module
