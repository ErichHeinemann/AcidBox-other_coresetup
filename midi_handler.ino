
uint8_t midi_clock_devider = 0;

uint8_t midi_seed = 0;

inline void MidiInit() {
  
#ifdef MIDI_VIA_SERIAL
  Serial.begin(115200, SERIAL_8N1);
#endif
#ifdef MIDI_VIA_SERIAL2
  pinMode( MIDIRX_PIN , INPUT_PULLDOWN);
  pinMode( MIDITX_PIN , OUTPUT);
  Serial2.begin( 31250, SERIAL_8N1, MIDIRX_PIN, MIDITX_PIN ); // midi port
#endif

#ifdef MIDI_VIA_SERIAL
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleProgramChange(handleProgramChange);

  // void  setHandleSystemExclusive (void(*fptr)(byte *array, byte size))
  //void  setHandleTimeCodeQuarterFrame (void(*fptr)(byte data))
  // MIDI.setHandleSongPosition (handleSongPosition);
  // void  setHandleSongSelect (void(*fptr)(byte songnumber))
  // void  setHandleTuneRequest (void(*fptr)(void))
  MIDI.setHandleClock(handleClock );
  MIDI.setHandleStart (handleStart );
  MIDI.setHandleContinue (handleContinue);
  MIDI.setHandleStop (handleStop);

  
  MIDI.begin(MIDI_CHANNEL_OMNI);
#endif
#ifdef MIDI_VIA_SERIAL2
  MIDI2.setHandleNoteOn(handleNoteOn);
  MIDI2.setHandleNoteOff(handleNoteOff);
  MIDI2.setHandleControlChange(handleCC);
  MIDI2.setHandlePitchBend(handlePitchBend);
  MIDI2.setHandleProgramChange(handleProgramChange);

// void  setHandleSystemExclusive (void(*fptr)(byte *array, byte size))
//void  setHandleTimeCodeQuarterFrame (void(*fptr)(byte data))
//   MIDI2.setHandleSongPosition (handleSongPosition);
// void  setHandleSongSelect (void(*fptr)(byte songnumber))
// void  setHandleTuneRequest (void(*fptr)(void))
  MIDI2.setHandleClock(handleClock );
  MIDI2.setHandleStart (handleStart );
  MIDI2.setHandleContinue (handleContinue);
  MIDI2.setHandleStop (handleStop);
  
  MIDI2.begin(MIDI_CHANNEL_OMNI);
#endif

}


inline void handleSongPosition( uint pos ){
  #ifdef DEBUG_MIDI
    DEBUG("MIDI SongPosition ");
    DEBUG( pos );
  #endif
}

inline void handleClock( ){
#ifdef DEBUG_MIDI
    DEBUG("MIDI Clock ");
#endif
  if( midi_playing && ext_midi_clock_master==true ){
    if( midi_clock_devider==0 ){
      set_midi_tick_ms( now - last_midi_tick );
      now = millis();
      last_midi_tick = now;
      do_midi_tick();
    }
    midi_clock_devider ++;
    if(  midi_clock_devider >5 ){
       midi_clock_devider=0;
    }
  }
}

inline void handleStart( ){
#ifdef DEBUG_MIDI
  DEBUG("MIDI Start ");
#endif
  // e.heinemann - function in AcidBanger
  set_ext_midi_clock_master( true );
  midi_step = -1;
  midi_clock_devider=0;
  init_midi(); // Acidbanger
}
  
inline void handleContinue( ){
  #ifdef DEBUG_MIDI
    DEBUG("MIDI Continue ");
  #endif
}
inline void handleStop(){ 
  #ifdef DEBUG_MIDI
    DEBUG("MIDI Stop ");
  #endif
  do_midi_stop();
  // E.Heinemann, Switch off ext MIDI Clock
  set_ext_midi_clock_master( false );
  set_bpm( bpm );  
}

inline void handleNoteOn(uint8_t inChannel, uint8_t inNote, uint8_t inVelocity) {
#ifdef DEBUG_MIDI
  DEB("MIDI note on ");
  DEBUG(inNote);
#endif
  if (inChannel == DRUM_MIDI_CHAN )         {Drums.NoteOn(inNote, inVelocity);}
  else if (inChannel == SYNTH1_MIDI_CHAN ){
    Synth1.on_midi_noteON(inNote, inVelocity);
    // Register pressed notes to use these notes
  }
  else if (inChannel == SYNTH2_MIDI_CHAN )  {Synth2.on_midi_noteON(inNote, inVelocity);}
  else if (inChannel = CTRL_CHAN ){ /*Process Keys as Control-Changes, ignore inVelocity */
     regmidi_note( inNote );
     if( inVelocity > 0 ){
       inVelocity=127;
       
       if( inNote < 48 ){
         switch( inNote%12 ){
          case 0:
            // toggle mute Synth 1
            mute_instrument(  0 );
           break; 
           // toggle mute Synth 2
            mute_instrument(  1 );
          case 1:
           // toggle Mute Kick
            mute_instrument(  2 );
           break; 
          case 2:
           // toggle Mute Snare
            mute_instrument(  3 );
           break; 
          case 3:
            mute_instrument(  4 );
           // toggle Mute Hihat
           break; 
          case 4:
            // toggle Mute Crash
            mute_instrument(  5 );
           break; 
          case 5:
           // toggle Mute Percussion
            mute_instrument( 6 );
           break; 
          case 6:
          // Toggle Mode - Jukebox-Performance or MIDI-Note-Mode
           break; 
          case 7:
           // Toggle Ramp-Automation on or off
           break; 
          case 8:
           // Next Drumset - Programchange
           // if we know the last set, then we could toggle between all
           break; 
          case 9:
           // Do a Break in Acidbanger
           do_break();
           break; 
          case 10:
           // Start at first step!!
           break; 
          case 11:
           // Start- or stop
           break; 
          default:
           break;            
         }

        }else{
         // Upper Note is used to change next Key-Seed
          
        } 
    
      }else{
         inVelocity=0;
      }
    
    
  }  
}


inline void handleNoteOff(uint8_t inChannel, uint8_t inNote, uint8_t inVelocity) {
  if (inChannel == DRUM_MIDI_CHAN )         {Drums.NoteOff(inNote);}
  else if (inChannel == SYNTH1_MIDI_CHAN )  {Synth1.on_midi_noteOFF(inNote, inVelocity);}
  else if (inChannel == SYNTH2_MIDI_CHAN )  {Synth2.on_midi_noteOFF(inNote, inVelocity);}

}

inline void handleCC(uint8_t inChannel, uint8_t cc_number, uint8_t cc_value) {
  switch (cc_number) { // global parameters yet set via ANY channel CCs
    case CC_ANY_COMPRESSOR:
      Comp.SetRatio(3.0f + cc_value * 0.307081f);
      DEBF("Set Comp Ratio %d\r\n", cc_value);
      break;
    case CC_ANY_DELAY_TIME:
      Delay.SetLength(cc_value * MIDI_NORM);
      break;
    case CC_ANY_DELAY_FB:
      Delay.SetFeedback(cc_value * MIDI_NORM);
      break;
    case CC_ANY_DELAY_LVL:
      Delay.SetLevel(cc_value * MIDI_NORM);
      break;
    case CC_ANY_RESET_CCS:
    case CC_ANY_NOTES_OFF:
    case CC_ANY_SOUND_OFF:
        if (inChannel == SYNTH1_MIDI_CHAN && millis()-last_reset>1000 ) {
#ifdef JUKEBOX
          do_midi_stop();
#endif
          Synth1.allNotesOff();
          Synth2.allNotesOff();
          last_reset = millis();
        }
      break;
#ifndef NO_PSRAM
    case CC_ANY_REVERB_TIME:
      Reverb.SetTime(cc_value * MIDI_NORM);
      break;
    case CC_ANY_REVERB_LVL:
      Reverb.SetLevel(cc_value * MIDI_NORM);
      break;
#endif
    default:
      if (inChannel == DRUM_MIDI_CHAN )         {Drums.ParseCC(cc_number, cc_value);}
      else if (inChannel == SYNTH1_MIDI_CHAN )  {Synth1.ParseCC(cc_number, cc_value);}
      else if (inChannel == SYNTH2_MIDI_CHAN )  {Synth2.ParseCC(cc_number, cc_value);}
  }
}

void handleProgramChange(uint8_t inChannel, uint8_t number) {
  if (inChannel == DRUM_MIDI_CHAN) {     Drums.SetProgram(number);  }
}

inline void handlePitchBend(uint8_t inChannel, int number) {
  if (inChannel == DRUM_MIDI_CHAN )         {Drums.PitchBend(number);}
  else if (inChannel == SYNTH1_MIDI_CHAN )  {Synth1.PitchBend(number);}
  else if (inChannel == SYNTH2_MIDI_CHAN )  {Synth2.PitchBend(number);}
}
