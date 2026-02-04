/*
 * @file cluster_test.cpp
 * @brief Instrument Cluster Test Mode Implementation
 * 
 * This module simulates CAN2004 messages for testing CAN2010 instrument cluster.
 * Based on existing cluster test implementation.
 * 
 * IMPORTANT: Messages are in CAN2004 format (simulating car messages) but are
 * sent to CAN1 (CAN2010 cluster). This mimics the normal adapter behavior where
 * CAN2004 messages from car (CAN0) are forwarded to CAN2010 devices (CAN1).
 */

#include <cluster_test.h>
#include <config.h>
#include <can_utils.h>

// External variables from main.cpp
extern struct can_frame canMsgSnd;
extern MCP2515 CAN1;
extern bool SerialEnabled;
extern bool debugGeneral;

// ============================================================================
// INTERNAL VARIABLES
// ============================================================================

static unsigned long testClusterLastUpdate = 0;
static const unsigned long testClusterUpdateInterval = 10;  // 10ms = 100Hz base

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void encodeRPM(int rpm, byte* high, byte* low) {
  unsigned int rpmEncoded = (unsigned int)(rpm / 0.125);
  *high = (rpmEncoded >> 8) & 0xFF;
  *low = rpmEncoded & 0xFF;
}

void encodeSpeed(int speed, byte* high, byte* low) {
  unsigned int speedEncoded = (unsigned int)(speed / 0.01);
  *high = (speedEncoded >> 8) & 0xFF;
  *low = speedEncoded & 0xFF;
}

void encodeOdometerBCD(unsigned long odo, byte* byte0, byte* byte1, byte* byte2) {
  if (odo > 999999) odo = 999999;
  
  int digit6 = (odo / 100000) % 10;  // 100000s
  int digit5 = (odo / 10000) % 10;   // 10000s
  int digit4 = (odo / 1000) % 10;     // 1000s
  int digit3 = (odo / 100) % 10;      // 100s
  int digit2 = (odo / 10) % 10;      // 10s
  int digit1 = odo % 10;              // 1s
  
  *byte0 = (digit6 << 4) | digit5;
  *byte1 = (digit4 << 4) | digit3;
  *byte2 = (digit2 << 4) | digit1;
}

void setTestScenario(byte scenario, int* speed, int* rpm, int* fuel) {
  switch (scenario) {
    case 0:  // 0x02 - All zeros
      *speed = 0;
      *rpm = 0;
      *fuel = 0;
      break;
    case 1:  // 0x12 - 10 km/h
      *speed = 10;
      *rpm = 0;
      *fuel = 0;
      break;
    case 2:  // 0x22 - 30 km/h, 500 RPM
      *speed = 30;
      *rpm = 500;
      *fuel = 0;
      break;
    case 3:  // 0x32 - 50 km/h, 1000 RPM, 1/4 fuel
      *speed = 50;
      *rpm = 1000;
      *fuel = 25;
      break;
    case 4:  // 0x42 - 70 km/h, 1500 RPM, 1/4 fuel
      *speed = 70;
      *rpm = 1500;
      *fuel = 13;
      break;
    case 5:  // 0x52 - 90 km/h, 2000 RPM, 1/4 fuel
      *speed = 90;
      *rpm = 2000;
      *fuel = 26;
      break;
    case 6:  // 0x62 - 110 km/h, 2500 RPM, 1/2 fuel
      *speed = 110;
      *rpm = 2500;
      *fuel = 38;
      break;
    case 7:  // 0x72 - 130 km/h, 3000 RPM, 1/2 fuel
      *speed = 130;
      *rpm = 3000;
      *fuel = 50;
      break;
    case 8:  // 0x82 - 150 km/h, 3500 RPM, 1/2 fuel
      *speed = 150;
      *rpm = 3500;
      *fuel = 63;
      break;
    case 9:  // 0x92 - 170 km/h, 4000 RPM, 3/4 fuel
      *speed = 170;
      *rpm = 4000;
      *fuel = 74;
      break;
    case 10: // 0xA2 - 190 km/h, 4500 RPM, 3/4 fuel
      *speed = 190;
      *rpm = 4500;
      *fuel = 87;
      break;
    case 11: // 0xB2 - 210 km/h, 5000 RPM, Full fuel
      *speed = 210;
      *rpm = 5000;
      *fuel = 100;
      break;
    case 12: // 0xC2 - 230 km/h, 5500 RPM, Full fuel
      *speed = 230;
      *rpm = 5500;
      *fuel = 100;
      break;
    case 13: // 0xD2 - 250 km/h, 6000 RPM, Full fuel
      *speed = 250;
      *rpm = 6000;
      *fuel = 100;
      break;
    case 14: // 0xE2 - 260 km/h, 6000 RPM, Full fuel
      *speed = 260;
      *rpm = 6000;
      *fuel = 100;
      break;
    case 15: // 0xF2 - 657 km/h (max), 6000 RPM, Full fuel
      *speed = 657;
      *rpm = 6000;
      *fuel = 100;
      break;
    default:
      *speed = 0;
      *rpm = 0;
      *fuel = 0;
  }
}

// ============================================================================
// MAIN FUNCTIONS
// ============================================================================

void clusterTestInit() {
  if (SerialEnabled) {
    Serial.println("========================================");
    Serial.println("INSTRUMENT CLUSTER TEST MODE ENABLED");
    Serial.println("========================================");
    Serial.println("Simulating CAN2004 messages for CAN2010 cluster");
    Serial.println("Based on existing cluster test implementation");
    Serial.print("Update rate: ");
    Serial.print(1000 / testClusterUpdateInterval);
    Serial.println(" Hz base frequency");
    Serial.println("========================================");
    Serial.print("Initial values:");
    Serial.print(" Scenario=");
    Serial.print(testScenario);
    Serial.print(", Speed=");
    Serial.print(testSpeed);
    Serial.print(" km/h, RPM=");
    Serial.print(testRPM);
    Serial.print(", Fuel=");
    Serial.print(testFuel);
    Serial.print("%, Odometer=");
    Serial.print(testOdometer);
    Serial.print(" km");
    Serial.println();
    Serial.println("========================================");
    Serial.println("Messages sent:");
    Serial.println("  0xB6 (RPM+Speed) - every 500ms");
    Serial.println("  0xF6 (Odometer) - every 500ms");
    Serial.println("  0x161 (Oil+Fuel) - every 500ms");
    Serial.println("  0x36 (Ign+Bright) - every 100ms");
    Serial.println("  0x128 (Lights1) - every 200ms");
    Serial.println("  0x168 (Lights2) - every 300ms");
    Serial.println("========================================");
  }
}

void clusterTestLoop() {
  unsigned long currentTime = millis();
  
  if (currentTime - testClusterLastUpdate >= testClusterUpdateInterval) {
    testClusterLastUpdate = currentTime;
    testClusterCounter++;
    
    // Reset counter at 600 (like original liczarka)
    if (testClusterCounter > 600) {
      testClusterCounter = 0;
    }
    
    // Auto-increment scenario if enabled
    if (testAutoIncrement && testClusterCounter == 0) {
      testScenario++;
      if (testScenario > 15) testScenario = 0;
      setTestScenario(testScenario, &testSpeed, &testRPM, &testFuel);
    }
    
    // Encode RPM and Speed
    byte rpm1, rpm2, speed1, speed2;
    encodeRPM(testRPM, &rpm1, &rpm2);
    encodeSpeed(testSpeed, &speed1, &speed2);
    
    // Encode odometer
    byte odo1, odo2, odo3;
    encodeOdometerBCD(testOdometer, &odo1, &odo2, &odo3);
    
    // Send 0xB6 - RPM + Speed (frequently, like original at liczarka == 500)
    // NOTE: Messages are in CAN2004 format (simulating car messages) but sent to CAN1 (CAN2010 cluster)
    if (testClusterCounter == 500) {
      canMsgSnd.can_id = 0xB6;
      canMsgSnd.can_dlc = 8;
      canMsgSnd.data[0] = rpm1;
      canMsgSnd.data[1] = rpm2;
      canMsgSnd.data[2] = speed1;
      canMsgSnd.data[3] = speed2;
      canMsgSnd.data[4] = 0x00;
      canMsgSnd.data[5] = 0x00;
      canMsgSnd.data[6] = 0x00;
      canMsgSnd.data[7] = 0xD0;
      CAN1.sendMessage(&canMsgSnd);
    }
    
    // Send 0xF6 - Odometer display + Ignition (frequently, like original at liczarka == 500)
    if (testClusterCounter == 500) {
      canMsgSnd.can_id = 0xF6;
      canMsgSnd.can_dlc = 8;
      canMsgSnd.data[0] = testIgnition ? 0x8E : 0x0E;  // Ignition bit
      canMsgSnd.data[1] = 0x80;
      canMsgSnd.data[2] = odo1;  // Odometer byte 0
      canMsgSnd.data[3] = odo2;  // Odometer byte 1
      canMsgSnd.data[4] = odo3;  // Odometer byte 2
      canMsgSnd.data[5] = 0xB6;
      canMsgSnd.data[6] = 0xFF;
      canMsgSnd.data[7] = 0x10;
      CAN1.sendMessage(&canMsgSnd);
    }
    
    // Send 0x161 - Oil temp + Fuel gauge (frequently, like original at liczarka == 500)
    if (testClusterCounter == 500) {
      canMsgSnd.can_id = 0x161;
      canMsgSnd.can_dlc = 7;
      canMsgSnd.data[0] = 0x00;
      canMsgSnd.data[1] = 0x00;
      canMsgSnd.data[2] = testOilTemp;  // Oil temperature
      canMsgSnd.data[3] = testFuel;    // Fuel gauge (0x00-0x64 = 0-100%)
      canMsgSnd.data[4] = 0x00;
      canMsgSnd.data[5] = 0x00;
      canMsgSnd.data[6] = 0xFF;
      CAN1.sendMessage(&canMsgSnd);
    }
    
    // Send 0x36 - Ignition + Brightness (less frequently, like original at liczarka == 100)
    if (testClusterCounter == 100) {
      canMsgSnd.can_id = 0x36;
      canMsgSnd.can_dlc = 8;
      canMsgSnd.data[0] = 0x0E;
      canMsgSnd.data[1] = 0x00;
      canMsgSnd.data[2] = 0x00;
      canMsgSnd.data[3] = 0x3F;  // Brightness (can be adjusted)
      canMsgSnd.data[4] = 0x01;
      canMsgSnd.data[5] = 0x00;
      canMsgSnd.data[6] = 0x00;
      canMsgSnd.data[7] = 0xA0;
      CAN1.sendMessage(&canMsgSnd);
    }
    
    // Send 0x128 - Dash lights 1 (less frequently, like original at liczarka == 200)
    if (testClusterCounter == 200) {
      canMsgSnd.can_id = 0x128;
      canMsgSnd.can_dlc = 8;
      canMsgSnd.data[0] = 0xFF;  // All lights on (for testing)
      canMsgSnd.data[1] = 0xFF;
      canMsgSnd.data[2] = 0x00;
      canMsgSnd.data[3] = 0x00;
      canMsgSnd.data[4] = 0xFE;
      canMsgSnd.data[5] = 0x11;
      canMsgSnd.data[6] = 0x38;
      canMsgSnd.data[7] = 0x00;
      CAN1.sendMessage(&canMsgSnd);
    }
    
    // Send 0x168 - Dash lights 2 (less frequently, like original at liczarka == 300)
    if (testClusterCounter == 300) {
      canMsgSnd.can_id = 0x168;
      canMsgSnd.can_dlc = 8;
      canMsgSnd.data[0] = 0xFF;  // All lights on (for testing)
      canMsgSnd.data[1] = 0x00;
      canMsgSnd.data[2] = 0x00;
      canMsgSnd.data[3] = 0xF3;
      canMsgSnd.data[4] = 0x03;
      canMsgSnd.data[5] = 0x00;
      canMsgSnd.data[6] = 0xF0;
      canMsgSnd.data[7] = 0x00;
      CAN1.sendMessage(&canMsgSnd);
    }
    
    // Auto-increment odometer if engine running (simulate driving)
    if (testIgnition && testClusterCounter == 500 && testSpeed > 0) {
      // Increment by 0.01 km (at 10Hz update, this simulates driving)
      testOdometer += 1;
      if (testOdometer > 999999) testOdometer = 0;
    }
    
    // Debug output (only if SerialEnabled and debugGeneral)
    if (SerialEnabled && debugGeneral && testClusterCounter == 0) {
      Serial.print("Cluster Test - Scenario: ");
      Serial.print(testScenario);
      Serial.print(", Speed: ");
      Serial.print(testSpeed);
      Serial.print(" km/h, RPM: ");
      Serial.print(testRPM);
      Serial.print(", Fuel: ");
      Serial.print(testFuel);
      Serial.print("%, Odo: ");
      Serial.print(testOdometer);
      Serial.println(" km");
    }
  }
}
