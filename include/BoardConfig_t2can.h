#pragma once

/**
 * @file BoardConfig_t2can.h
 * @brief Pin definitions for LilyGO T2CAN board (ESP32-S3 with dual MCP2515)
 * 
 * This file contains all hardware pin definitions specific to the LilyGO T2CAN board.
 * Modify these definitions if you're using a different board configuration.
 */

// SPI pins for MCP2515 CAN controllers
#define BOARD_SPI_INSTANCE SPI2_HOST
#define BOARD_SCK_PIN  12  // SPI Clock
#define BOARD_MISO_PIN 13  // SPI Master In Slave Out
#define BOARD_MOSI_PIN 11  // SPI Master Out Slave In

// CAN1 (destination) - First MCP2515 (connected to vehicle CAN2004 bus)
#define BOARD_CAN1_CS_PIN 10  // Chip Select for first MCP2515

// CAN2 (source) - Second MCP2515 (connected to CAN2010 device)
#define BOARD_CAN2_CS_PIN 14  // Chip Select for second MCP2515

// I2C pins for RTC (DS1307/DS3231) - QWIIC interface on T2CAN
#define BOARD_SDA_PIN 8   // I2C Data
#define BOARD_SCL_PIN 9   // I2C Clock
