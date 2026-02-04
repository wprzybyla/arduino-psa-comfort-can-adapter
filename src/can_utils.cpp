/*
Copyright 2019-2022, Ludwig V. <https://github.com/ludwig-v>
Copyright 2021, Nick V. (V3nn3tj3) <https://github.com/v3nn3tj3>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#include <can_utils.h>
#include <TimeLib.h>

// External variables
extern struct can_frame canMsgSnd;
extern MCP2515 CAN1;
extern bool SerialEnabled;
extern int alertsCache[];
extern byte alertsParametersCache[];

void sendPOPup(bool present, int id, byte priority, byte parameters) {
  bool clear = false;
  byte firstEmptyPos = 8;

  for (int i = 0; i < 8; i++) {
    if (alertsCache[i] == id) {
      if (!present) { // Remove from cache and clear popup
        alertsCache[i] = alertsParametersCache[i] = firstEmptyPos = 0;
        clear = true;
        break;
      } else if (parameters == alertsParametersCache[i]) { // Already sent
        return;
      } else {
        return sendPOPup(false, id, priority, 0x00); // Clear previous popup first
      }
    } else if (alertsCache[i] == 0 && firstEmptyPos >= 8) {
      firstEmptyPos = i;
    }
  }

  if (firstEmptyPos >= 8) {
    return; // Avoid overflow    
  }
  if (!present && !clear) {
    return;
  } else if (!clear) {
    alertsCache[firstEmptyPos] = id;
    alertsParametersCache[firstEmptyPos] = parameters;

    if (SerialEnabled && present) {
      Serial.print("Notification sent with message ID: ");
      Serial.println(id);
    }
  }

  if (priority > 14) {
    priority = 14;
  }

  if (present) {
    canMsgSnd.data[0] = highByte(id); 
    canMsgSnd.data[1] = lowByte(id);
    bitWrite(canMsgSnd.data[0], 7, present); // New message
  } else { // Close Popup
    canMsgSnd.data[0] = 0x7F; 
    canMsgSnd.data[1] = 0xFF;
  }
  canMsgSnd.data[2] = priority; // Priority (0 > 14)
  bitWrite(canMsgSnd.data[2], 7, 1); // Destination: NAC / EMF / MATT
  bitWrite(canMsgSnd.data[2], 6, 1); // Destination: CMB
  canMsgSnd.data[3] = parameters; // Parameters
  canMsgSnd.data[4] = 0x00; // Parameters
  canMsgSnd.data[5] = 0x00; // Parameters
  canMsgSnd.data[6] = 0x00; // Parameters
  canMsgSnd.data[7] = 0x00; // Parameters
  canMsgSnd.can_id = 0x1A1;
  canMsgSnd.can_dlc = 8;
  CAN1.sendMessage( & canMsgSnd);

  return;
}

int daysSinceYearStartFct() {
  // Given a day, month, and year (4 digit), returns
  // the day of year. Errors return 999.
  int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  // Check if it is a leap year, this is confusing business
  // See: https://support.microsoft.com/en-us/kb/214019
  if (year()%4  == 0) {
    if (year()%100 != 0) {
      daysInMonth[1] = 29;
    }
    else {
      if (year()%400 == 0) {
        daysInMonth[1] = 29;
      }
    }
   }

  int doy = 0;
  for (int i = 0; i < month() - 1; i++) {
    doy += daysInMonth[i];
  }

  doy += day();
  return doy;
}

byte checksumm_0E6(const byte* frame)
{
    /* Autors:
        organizer of the bacchanal: styleflava
        algorithm: Ilia
        code: Pepelxl
    */
    static byte iter = 0;
    byte cursumm = 0;
    for (byte i = 0; i < 7; i++)
    {
        cursumm += (frame[i] >> 4) + (frame[i] & 0x0F);
    }
    cursumm += iter;
    cursumm = ((cursumm ^ 0xFF) - 3) & 0x0F;
    cursumm ^= iter << 4;
    iter++;
    if (iter >= 16) iter = 0;
    return cursumm;
}
