// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Mon Oct 17 12:31:42 2022
//  Last Modified : <221026.1140>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2022  Robert Heller D/B/A Deepwoods Software
//			51 Locke Hill Road
//			Wendell, MA 01379-9728
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// 
//
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";


#include <Arduino.h>
#include <soc/rtc.h>
#include "esp_err.h"

#include <WiFi.h>
#include "SPIFFS.h"
#include <Button.h>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputDAC.h"
#include "AudioOutputI2S.h"

// Audio from uSD.
AudioFileSourceSD sdFile;
// WAVE files.
AudioGeneratorWAV generator;
// Out to DAC.
//AudioOutputDAC amplifier;
AudioOutputI2S amplifier(0,AudioOutputI2S::INTERNAL_DAC);


#ifdef USE_SR04
// SR04 Data pins
const int TRIG_PIN = 2;
const int ECHO_PIN = 4;
// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

// Distance thresholds
const float CLOSE_INCHES = 2.5;
const float NEAR_INCHES  = 5.0;
const float FAR_INCHES   = 10.0;
const float VERY_FAR_INCHES = 15.0;
const float DISTANT_INCHES  = 30.0;

bool IsClose = false;
#else
Button b4(4);

const int READY_LED = 2;
#endif

void setup() {
    Serial.begin(115200);
#ifdef USE_SR04
    // The Trigger pin will tell the sensor to range find
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIG_PIN, LOW);
#else
    b4.begin();
    pinMode(READY_LED, OUTPUT);
    digitalWrite(READY_LED,HIGH);
#endif
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    // Init DAC
    //amplifier.init();
    //Serial.println("amplifier init'ed");
    // Init (mount) uSD
    SD.begin();
    Serial.println("SD mounted");
}

#ifdef USE_SR04
// Check for change in proximity
bool CheckProx()
{
    unsigned long t1;
    unsigned long t2;
    unsigned long pulse_width;
    float cm;
    float inches;
    
    Serial.println("*** CheckProx()");
    // Hold the trigger pin high for at least 10 us
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Wait for pulse on echo pin
    while ( digitalRead(ECHO_PIN) == 0 )
    {
    }
    
    // Measure how long the echo pin was held high (pulse width)
    // Note: the micros() counter will overflow after ~70 min
    t1 = micros();
    while ( digitalRead(ECHO_PIN) == 1)
    {
        if (micros()-t1 > MAX_DIST) break;
    }
    t2 = micros();
    pulse_width = t2 - t1;
    Serial.print("*** pulse_width = ");Serial.println(pulse_width);
    // Calculate distance in centimeters and inches. The constants
    // are found in the datasheet, and calculated from the assumed speed 
    //of sound in air at sea level (~340 m/s).
    inches = pulse_width / 148.0;
    
    Serial.print("*** inch = ");Serial.println(inches);
    // If was close, have moved away?
    if (IsClose && inches > FAR_INCHES)
    {
        IsClose = false;
        return false;
    }
    // If wasn't close, have moved closer?
    if (!IsClose && inches < NEAR_INCHES)
    {
        IsClose = true;
        return true;
    }
    // Nothing to see here (no change).
    return false;
}
#endif

// Choose a random track.
const char *RandomTrack()
{
    static char buffer[128];
    unsigned tracknum = (unsigned) ((rand()/(double)RAND_MAX)*74) + 1;
    snprintf(buffer,sizeof(buffer),"/Track%02u.wav",tracknum);
    Serial.println(buffer);
    return buffer;
}
        
        

void loop() {
    // Tight loop playing audio.
    if (generator.isRunning()) {
        if (!generator.loop()) 
        {
            generator.stop();
            if (sdFile.isOpen()) sdFile.close();
            digitalWrite(LED_BUILTIN,LOW);
        }
    }
    else
    {
        // Otherwise looser loop checking distance.
#ifdef USE_SR04
        if (CheckProx())
#else
        digitalWrite(READY_LED,HIGH);
        if (b4.pressed())
#endif
        {
#ifndef USE_SR04
            while (!b4.released());
            digitalWrite(READY_LED,LOW);
#endif
            sdFile.open(RandomTrack());
            generator.begin(&sdFile, &amplifier);
            digitalWrite(LED_BUILTIN,HIGH);
        }
        else
        {
            // Check again in a second. (Delay only when idle.)
            delay(1000); 
        }
    }
}    
