/**
 * NeoPixel Clock with NTP Synchronization
 * This script controls a NeoPixel LED ring to display the current time,
 * synchronized via NTP (Network Time Protocol).
 *
 * Created on: 04.11.2023
 * Last updated on: 04.11.2023 V1.0
 *
 * Author: Alf Müller / purecrea gmbh Switzerland / www.bastelgarage.ch
 *
 * Copyright (c) 2023 Alf Müller. All rights reserved.
 * Redistribution, distribution, and modification of this script are allowed
 * under the condition of attribution and without any licensing fees.
 */
#include <ESP8266WiFi.h>
#include "WiFiManager.h"                    // https://github.com/tzapu/WiFiManager
#include <Timezone.h>                       // https://github.com/JChristensen/Timezone
#include <TimeLib.h>                        // https://github.com/PaulStoffregen/Time
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#define PIN D6
#define NUMPIXELS 120

// NeoPixel library initialization
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// NTP client initialization
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);

// Define Central Europe timezone
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Daylight Saving Time: Last Sunday in March at 2 AM +2 hours
TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};    // Standard Time: Last Sunday in October at 3 AM +1 hour
Timezone CE(CEST, CET);

// Variable to store the last minute
int lastMinute = -1;

void setup() {
  pixels.begin();
  WiFiManager wifiManager;
  wifiManager.autoConnect("NeoPixelClock");
  timeClient.begin();
}

void loop() {
  // Update NTP time
  timeClient.update();

  // Get current time in UTC
  time_t utc = timeClient.getEpochTime();

  // Convert UTC time to local time
  time_t local = CE.toLocal(utc);

  // Get current hour, minute, and second from NTP time
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();

  // Set the LEDs based on the current time
  setTimeOnLEDs(hours, minutes, 0, 0, 55, 0, 55, 0);  // Blue for minutes, green for hours
  delay(1000);                                        // Wait one second before the next iteration begins
}

void setTimeOnLEDs(int hours, int minutes, byte rMinutes, byte gMinutes, byte bMinutes, byte rHours, byte gHours, byte bHours) {
  pixels.clear();  // Turn off all LEDs

  // Set the minute LEDs
  for (int i = 0; i < minutes; i++) {
    int minutesLedIndex = (i + 29) % 60;                                               // Start at LED 29 for the first minute
    pixels.setPixelColor(minutesLedIndex, pixels.Color(rMinutes, gMinutes, bMinutes));  // Set color for each minute
  }

  // Calculate the number of LEDs that should be on for the hours
  // Make sure hours are in 12-hour format
  hours = hours % 12;
  hours = hours ? hours : 12;                // Convert 0 hours (midnight) to 12
  int hoursCount = hours * 5 + minutes / 12;  // Each hour has 5 LEDs, each minute adds 1/12 of an hour

  // Set the hour LEDs
  for (int i = 0; i < hoursCount; i++) {
    int hoursLedIndex = (90 - i) % 60 + 60;                                           // Start at LED 91 for the first hour
    pixels.setPixelColor(hoursLedIndex, pixels.Color(rHours, gHours, bHours));          // Set color for hours
  }

  pixels.show();  // Update the strip to show the changes
}
