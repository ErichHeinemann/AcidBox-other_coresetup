/*
 * This module is run adc with a multiplexer
 * tested with ESP32 Audio Kit V2.2
 * Only tested with 8 inputs
 *
 * Define your adc mapping in the lookup table
 *
 * Reference: https://youtu.be/l8GrNxElRkc
 *
    Copyright (C) 2021  Marcel Licence

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef __CDT_PARSER__
#include <cdt.h>
#endif

#define ADC_ENABLED

#ifdef ADC_ENABLED
#define ADC_MUL_S0_PIN 12
#define ADC_MUL_S1_PIN 13
#define ADC_MUL_S2_PIN 14
#define ADC_INPUTS 8
#define ADC_MUL_SIG_PIN 34
#define ADC_OVERSAMPLING 2
#define ADC_TO_MIDI_LOOKUP_SIZE 8 /* should match ADC_INPUTS AND CONNECTED ADDRESS-PINS! */
// #define ADC_DEBUG_CHANNEL0_DATA
byte last_adc_input = 0;
#endif

// Pages for Pots .. first Pot changes the Page
uint8_t page =0;

// ADC ist mainly used to get analog pots to modifiy internal values


uint8_t lastSendVal[ADC_TO_MIDI_LOOKUP_SIZE]; /* define ADC_TO_MIDI_LOOKUP_SIZE in top level file */
#define ADC_THRESHOLD   2
// (1.0f/200.0f)

//#define ADC_DEBUG_CHANNEL0_DATA

// static float adcChannelValue[ADC_INPUTS];
static byte adcChannelValue[ADC_INPUTS];

void AdcMul_Init(void)
{
    for (int i = 0; i < ADC_INPUTS; i++)
    {
        adcChannelValue[i] = 75;
    }

    memset(lastSendVal, 0xFF, sizeof(lastSendVal));

    analogReadResolution(8); // 10
    analogSetAttenuation(ADC_11db);
#if 0
    analogSetCycles(1);
#endif
    analogSetClockDiv(1);

    adcAttachPin(ADC_MUL_SIG_PIN);
    pinMode(ADC_MUL_S0_PIN, OUTPUT);
#if ADC_INPUTS > 2
    pinMode(ADC_MUL_S1_PIN, OUTPUT);
#endif
#if ADC_INPUTS > 4
    pinMode(ADC_MUL_S2_PIN, OUTPUT);
#endif
#if ADC_INPUTS > 8
    pinMode(ADC_MUL_S3_PIN, OUTPUT);
#endif
}

void AdcMul_Process(void)
{
   // DEB("AdcMul Process\r\n");
    static int16_t readAccu = 0;
    static float adcMin = 0;//4000;
    static float adcMax = 420453;//410000;

   for (int k = 0; k < 2; k++){
//    for (int j = 0; j < ADC_INPUTS; j++)
    last_adc_input = last_adc_input +1;
    if( last_adc_input >= ADC_INPUTS ){ 
      last_adc_input=0;
    }
     //{
      int j = last_adc_input;
        digitalWrite(ADC_MUL_S0_PIN, ((j & (1 << 0)) > 0) ? HIGH : LOW);
#if ADC_INPUTS > 2
        digitalWrite(ADC_MUL_S1_PIN, ((j & (1 << 1)) > 0) ? HIGH : LOW);
#endif
#if ADC_INPUTS > 4
        digitalWrite(ADC_MUL_S2_PIN, ((j & (1 << 2)) > 0) ? HIGH : LOW);
#endif
#if ADC_INPUTS > 8
        digitalWrite(ADC_MUL_S3_PIN, ((j & (1 << 3)) > 0) ? HIGH : LOW);
#endif
        // give some time for transition
        delay(1);
// #endif
        readAccu = 0;
        for (int i = 0 ; i < ADC_OVERSAMPLING; i++)
        {
            readAccu += analogRead( ADC_MUL_SIG_PIN );
        }
        readAccu = ( readAccu / ADC_OVERSAMPLING ); //  >> 3; 

        // We have to check if the old value was crossed ... or if we use direct control?
        if( ( readAccu - ADC_THRESHOLD ) >  adcChannelValue[j] || readAccu +ADC_THRESHOLD  <  adcChannelValue[j] ){
          adcChannelValue[j] = (adcChannelValue[j] + readAccu ) / 2 ;
          if( j==0 ){
            // change Page
            page =  round(  adcChannelValue[j] / 10 );
          }
          // send Change  
#ifdef DEBUG_ADC          
          if( j==0 ){
            DEBF( "Channel %d new val: %d \n\r", j, adcChannelValue[j] );
          }  
#endif
          switch(  page ){
            case 0:
              // Switch ADC !!!  Synth 1, Synth 2 etc... Synth-Sound
              switch( j ){
                case 1:
                  // Cutoff
                  DEB("Change Cutoff\n");
                  handleCC( SYNTH1_MIDI_CHAN , CC_303_CUTOFF,  adcChannelValue[j] );
                  break;
                case 2:
                  // Reso
                  DEB("Change RESO\n");
                  handleCC( SYNTH1_MIDI_CHAN, CC_303_RESO,  adcChannelValue[j] );
                  break;
                case 3:
                  // Attack
                  DEB("Change ATTACK\n");
                  handleCC( SYNTH1_MIDI_CHAN, CC_303_ATTACK,  adcChannelValue[j] );
                  break;
                case 4:
                  // Decay
                  DEB("Change DECAY\n");
                  handleCC( CC_303_DECAY, CC_303_ATTACK,  adcChannelValue[j] );
                  break;
                case 5:
                  // Waveform
                  DEB("Change Waveform\n");
                  handleCC( CC_303_WAVEFORM, CC_303_ATTACK,  adcChannelValue[j] );
                  break;
                case 6:
                  // CC_303_VOLUME
                  DEB("Change DECAY\n");
                  handleCC( CC_303_DECAY, CC_303_ATTACK,  adcChannelValue[j] );
                  break;
              }
/*#define CC_303_PORTATIME    5
#define CC_303_VOLUME       7
#define CC_303_PORTAMENTO   65
#define CC_303_PAN          10
#define CC_303_WAVEFORM     70
#define CC_303_RESO         71
#define CC_303_CUTOFF       74
#define CC_303_ATTACK       73
#define CC_303_DECAY        72
#define CC_303_ENVMOD_LVL   75
#define CC_303_ACCENT_LVL   76
#define CC_303_REVERB_SEND  91
#define CC_303_DELAY_SEND   92
#define CC_303_DISTORTION   94
#define CC_303_OVERDRIVE    95
#define CC_303_SATURATOR    128
*/
              
             case 1: // Page Synth1 Effect and Waveform 
              switch( j ){
                case 1:
                  // Waveform
                  break;
                case 2:
                  // Pan
                  break;
                case 3:
                  // Volume
                  break;
                case 4:
                  // Delay
                  break;
              }
              
              break;
            
          } 
          
        }

#ifdef ADC_DEBUG_CHANNEL0_DATA
        if( j==0 ){
          DEB( adcChannelValue[j] );
          DEB( "\r\n" );  
        }  
#endif        

    }
}

byte *AdcMul_GetValues(void)
{
    return adcChannelValue;
}
