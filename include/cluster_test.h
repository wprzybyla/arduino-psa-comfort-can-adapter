#pragma once

/**
 * @file cluster_test.h
 * @brief Instrument Cluster Test Mode for CAN2010
 * 
 * This module provides testing functionality for CAN2010 instrument cluster
 * without requiring a connected car or BSI. It simulates CAN2004 messages
 * and sends them to the CAN2010 cluster.
 * 
 * Based on existing cluster test implementation.
 */

#include <Arduino.h>
#include <mcp2515.h>

// ============================================================================
// CONFIGURATION VARIABLES (set in main.cpp)
// ============================================================================
// These variables should be declared as extern in main.cpp
// and defined in cluster_test.cpp

extern bool testClusterMode;              // Enable/disable test mode
extern unsigned long testClusterCounter;  // Internal counter for timing
extern int testSpeed;                     // Vehicle speed (km/h)
extern int testRPM;                       // Engine RPM
extern int testFuel;                      // Fuel level (0-100%)
extern unsigned long testOdometer;        // Odometer (km)
extern bool testIgnition;                 // Ignition state
extern byte testScenario;                 // Test scenario (0-15)
extern bool testAutoIncrement;           // Auto-cycle scenarios
extern int testOilTemp;                   // Oil temperature

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

/**
 * @brief Initialize cluster test mode
 * Call this in setup() if testClusterMode is enabled
 */
void clusterTestInit();

/**
 * @brief Main cluster test loop
 * Call this in loop() before main CAN processing
 */
void clusterTestLoop();

/**
 * @brief Encode RPM to CAN format (0.125 RPM units, 2 bytes)
 * @param rpm RPM value
 * @param high Pointer to store high byte
 * @param low Pointer to store low byte
 */
void encodeRPM(int rpm, byte* high, byte* low);

/**
 * @brief Encode speed to CAN format (0.01 km/h units, 2 bytes)
 * @param speed Speed value in km/h
 * @param high Pointer to store high byte
 * @param low Pointer to store low byte
 */
void encodeSpeed(int speed, byte* high, byte* low);

/**
 * @brief Encode odometer to BCD format (3 bytes)
 * @param odo Odometer value in km
 * @param byte0 Pointer to store first byte (100000s and 10000s)
 * @param byte1 Pointer to store second byte (1000s and 100s)
 * @param byte2 Pointer to store third byte (10s and 1s)
 */
void encodeOdometerBCD(unsigned long odo, byte* byte0, byte* byte1, byte* byte2);

/**
 * @brief Set test values based on scenario (matches original switch cases)
 * @param scenario Scenario number (0-15)
 * @param speed Pointer to store speed value
 * @param rpm Pointer to store RPM value
 * @param fuel Pointer to store fuel level
 */
void setTestScenario(byte scenario, int* speed, int* rpm, int* fuel);
