// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Mon Oct 17 13:06:33 2022
//  Last Modified : <221026.0857>
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

#ifndef __AUDIOOUTPUTDAC_H
#define __AUDIOOUTPUTDAC_H

#include <AudioOutput.h>
#pragma once
#include "driver/dac.h"

class AudioOutputDAC : public AudioOutput
{
public:
    AudioOutputDAC() : micros_(1000000/44100) {}
    virtual ~AudioOutputDAC() {};
    virtual bool begin() {return true;}
    // Initialize the analog output channel.
    esp_err_t init()
    {
        esp_err_t err = dac_output_enable(DAC_CHANNEL_1);
        if (err != ESP_OK) {
            Serial.print("dac_output_enable failed: ");
            Serial.println(err);
            return err;
        }
        return ESP_OK;
    }
    // Consume a pair (left and right) samples.
    virtual bool ConsumeSample(int16_t samples[2])
    {
        int16_t l, r, mono;
        l = samples[0];
        r = samples[1];
        if (r == 0) {
            // mono on l, (r is 0);
            mono = l;
        } else {
            // Average left and right
            mono = (l+r)/2;
        }
        uint16_t sample = mono;
        // Shift down to 12 bits and send to DAC.
        esp_err_t err = dac_output_voltage(DAC_CHANNEL_1,sample>>4);
        // Error check.
        if (err != ESP_OK) {
            Serial.print("dac_output_voltage failed: ");
            Serial.println(err);
            return false;
        }
        delayMicroseconds(micros_);
        return true;
    }
    // We can stop anytime you like.
    virtual bool stop() {
        return true;
    }
    virtual bool SetRate(int hz)
    {
        micros_ = 1000000 / hz;
        return true;
    }
private:
    uint32_t micros_;
};

#endif // __AUDIOOUTPUTDAC_H

