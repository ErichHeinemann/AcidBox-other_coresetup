# AcidBox
ESP32 headless acid combo of tb303 + tb303 + tr808 like synths. Filter cutoff, reso, env mod, accent, wavefolder, overdrive within each 303, per-instrument tunings, hi-pass/lo-pass filter and bitcrusher in drums, send to reverb, delay and master compression.
All MIDI driven. 44100, 16bit stereo I2S output to the external DAC or 8bit to the built-in DAC. No indication. Uses both cores of ESP32. Cheap ~$10.
Consult with midi_config.h to find out and to set up MIDI continous control messages.

>>> This is only a TEST-Repo to check some functions with slightly different functionality
a) Analog Multiplexer HC to add up to 8 Pots by using 3 GPIOs for Adresses
b) Adding some Buttons to free ports
c) MIDI-Sync
d) Synth-Control (Instrument-Mute) via MIDI-Keyboard (MIDI-Notes)


Don´t use this repo, please use the repo from COPYCH!!

Best Regards
Erich




# MIDI Control
For the time being the following list of MIDI continious controllers is available:

    #define CC_303_PORTATIME    5   // affects gliding time
    #define CC_303_VOLUME       7   // mix volume
    #define CC_303_PORTAMENTO   65  // gliding on/off
    #define CC_303_PAN          10  // pano
    #define CC_303_WAVEFORM     70  // Blend between square and saw
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

    // 808 Drums MIDI CC
    #define CC_808_VOLUME       7
    #define CC_808_PAN          10
    #define CC_808_RESO         71
    #define CC_808_CUTOFF       74  // Note that this filter's behaviour differs from the 303's, 64-127 means HP-, and 0-63 LP-filtering. 'Untouched' is at ~64.  
    #define CC_808_REVERB_SEND  91
    #define CC_808_DELAY_SEND   92
    #define CC_808_DISTORTION   94  // BitCrusher
    #define CC_808_BD_TONE      21  // Bass Drum tone control
    #define CC_808_BD_DECAY     23  // Bass Drum envelope decay
    #define CC_808_BD_LEVEL     24  // Bass Drum mix level
    #define CC_808_SD_TONE      25  // Snare Drum tone control
    #define CC_808_SD_SNAP      26  // Snare Drum envelope decay
    #define CC_808_SD_LEVEL     29  // Snare Drum mix level
    #define CC_808_CH_TUNE      61  // Closed Hat tone control
    #define CC_808_CH_LEVEL     63  // Closed Hat mix level
    #define CC_808_OH_TUNE      80  // Open Hat tone control
    #define CC_808_OH_DECAY     81  // Open Hat envelope decay
    #define CC_808_OH_LEVEL     82  // Open Hat mix level

    // Global 
    #define CC_ANY_COMPRESSOR   93
    #define CC_ANY_DELAY_TIME   84  // delay time
    #define CC_ANY_DELAY_FB     85  // delay feedback level
    #define CC_ANY_DELAY_LVL    86  // delay mix level
    #define CC_ANY_REVERB_TIME  87  // rebverb time
    #define CC_ANY_REVERB_LVL   88  // reverb mix level
    #define CC_ANY_RESET_CCS    121
    #define CC_ANY_NOTES_OFF    123
    #define CC_ANY_SOUND_OFF    120


# 
<img src="https://github.com/copych/AcidBox/blob/main/media/2022-12-13%2015-44-53.JPG" width=100% > ESP32 proto


<img src="https://github.com/copych/AcidBox/blob/main/hardware/2023-03-28%20at%2009.19.49.jpeg" width=100% > ESP32s3 proto
