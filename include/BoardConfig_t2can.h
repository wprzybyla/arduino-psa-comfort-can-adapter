#pragma once

// SPI dla MCP2515
#define BOARD_SPI_INSTANCE SPI2_HOST
#define BOARD_SCK_PIN 12
#define BOARD_MISO_PIN 13
#define BOARD_MOSI_PIN 11

// CAN1 (destination) - pierwszy MCP2515
#define BOARD_CAN1_CS_PIN 10
#define BOARD_CAN1_RX_PIN 6  // MCP2515 INT pin (nie używany bezpośrednio, ale zdefiniowany dla kompatybilności)
#define BOARD_CAN1_TX_PIN 7  // MCP2515 INT pin (nie używany bezpośrednio, ale zdefiniowany dla kompatybilności)

// CAN2 (source) - drugi MCP2515
#define BOARD_CAN2_CS_PIN 14  // Zakładam standardowy pin CS dla drugiego MCP2515 (sprawdź dokumentację swojej płytki)
#define BOARD_CAN2_RX_PIN 6   // MCP2515 INT pin (nie używany bezpośrednio)
#define BOARD_CAN2_TX_PIN 7   // MCP2515 INT pin (nie używany bezpośrednio)

// SPI dla CAN2 (drugi MCP2515) - współdzielone z CAN1
#define BOARD_CAN2_MISO_PIN BOARD_MISO_PIN
#define BOARD_CAN2_MOSI_PIN BOARD_MOSI_PIN
#define BOARD_CAN2_SCK_PIN BOARD_SCK_PIN

// Dla kompatybilności z istniejącym kodem (jeśli nie zdefiniowano BOARD_CAN2_CS_PIN)
#ifndef BOARD_CS_PIN
    #define BOARD_CS_PIN BOARD_CAN2_CS_PIN
#endif

// I2C dla RTC (DS3231) - QWIIC interface na T-2CAN
#define BOARD_SDA_PIN 8   // QWIIC SDA na T-2CAN (sprawdź dokumentację)
#define BOARD_SCL_PIN 9   // QWIIC SCL na T-2CAN (sprawdź dokumentację)

// LED - T-2CAN może nie mieć wbudowanego LED, użyj zewnętrznego lub pozostaw bez zmian
#define BOARD_LED_PIN 2   // Standardowy pin LED dla ESP32-S3 (sprawdź dokumentację swojej płytki)

// Typy CAN
#define BOARD_CAN1_TYPE 2 // 1 - built-in ESP32 CAN, 2 - MCP2515
#define BOARD_SECOND_CAN_TYPE 2 // 1 - built-in ESP32 CAN, 2 - MCP2515

// Typy protokołów
#define BOARD_PROTOCOL_TYPES 2 // 1 - VAN-CAN, 2 - CAN-CAN, 3 - VAN-CAN + CAN-CAN

// Obsługiwane protokoły (tylko CAN-CAN dla T-2CAN)
#define BOARD_SUPPORTED_SOURCE_PROTOCOLS      { static_cast<uint8_t>(ProtocolType::AEE2004), static_cast<uint8_t>(ProtocolType::BmwCan) }
#define BOARD_SUPPORTED_DESTINATION_PROTOCOLS { static_cast<uint8_t>(ProtocolType::AEE2010) }
