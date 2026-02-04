#pragma once

/**
 * @file can_utils.h
 * @brief CAN bus utility functions
 * 
 * This header declares helper functions for CAN bus operations including:
 * - Checksum calculations
 * - Popup notification management
 * - Date/time calculations
 */

#include <Arduino.h>
#include <mcp2515.h>

// Function declarations

/**
 * @brief Calculate checksum for CAN frame 0xE6
 * @param frame Pointer to 7-byte frame data
 * @return Calculated checksum byte
 */
byte checksumm_0E6(const byte* frame);

/**
 * @brief Send or clear a popup notification on CAN2010 device
 * @param present True to show popup, false to clear
 * @param id Alert/notification ID
 * @param priority Priority level (0-14)
 * @param parameters Additional parameters for the notification
 */
void sendPOPup(bool present, int id, byte priority, byte parameters);

/**
 * @brief Calculate number of days since start of current year
 * @return Number of days since January 1st
 */
int daysSinceYearStartFct();

// External variables needed by these functions
extern struct can_frame canMsgSnd;
extern MCP2515 CAN1;
extern bool SerialEnabled;
extern int alertsCache[];
extern byte alertsParametersCache[];
