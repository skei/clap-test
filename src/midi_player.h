#ifndef midi_player_included
#define midi_player_included
//----------------------------------------------------------------------

// UNTESTED !!

// "plays" a MidiSequence
// keeps track of 'blocksize' window into event stream

#include "midi_file.h"

//----------------------------------------------------------------------

struct PlayerTrack {
  //MidiTrack*  miditrack = nullptr;
  uint32_t    next_event = 0;
};

typedef std::vector<PlayerTrack*> PlayerTracks;

//----------------------------------------------------------------------

class MidiPlayer {

  MidiFile*     MMidiFile   = nullptr;
  MidiSequence* MSequence   = nullptr;
  uint32_t      MSamplePos  = 0;
  PlayerTracks  MTracks     = {};

  float         MSampleRate       = 44100.0;
  float         MSecondsPerSample = (1.0 / MSampleRate);
  float         MCurrentTime      = 0.0;

//------------------------------
public:
//------------------------------

  MidiPlayer() {
  }

  //----------

  ~MidiPlayer() {
  }

//------------------------------
public:
//------------------------------

  void initialize(MidiFile* AMidiFile, float ASampleRate=44100.0, float AStartPos=0.0) {

    MMidiFile = AMidiFile;
    MSequence = AMidiFile->getMidiSequence();

    MSampleRate = ASampleRate;
    MSecondsPerSample = 1.0 / MSampleRate;
    MCurrentTime = AStartPos;

    for (uint32_t i=0; i<MSequence->num_tracks; i++) {
      PlayerTrack* track = new PlayerTrack();
      //track->miditrack = MSequence->tracks[i];
      track->next_event = 0;
      MTracks.push_back(track);
    }

  }

  //----------

  void cleanup() {
    for (uint32_t i=0; i<MTracks.size(); i++) {
      delete MTracks[i];
      MTracks[i] = nullptr;
    }
  }

  //----------

  // process 1 sample

  void process(uint32_t ASampleRate) {
    for (uint32_t t=0; t<MTracks.size(); t++) {
      if (MSequence) {
        MidiTrack*    miditrack   = MSequence->tracks[t];
        if (miditrack) {
          PlayerTrack*  playertrack = MTracks[t];
          if (playertrack) {
            // valid next event index? (not end of track)
            if (playertrack->next_event < miditrack->events.size()) {

              // all events since last time..
              while (miditrack->events[playertrack->next_event]->time <= MCurrentTime) {
                MidiEvent* next_event = miditrack->events[playertrack->next_event];
                //handlee next_event
                printf("event.. track %i time %.2f : %02x %02x %02x\n",t,next_event->time,next_event->msg1,next_event->msg2,next_event->msg3);
                playertrack->next_event += 1;
                // last event?
                if (playertrack->next_event >= miditrack->events.size()) {
                  playertrack = nullptr;
                  MTracks[t] = nullptr;
                  break;
                }
              } // while time < currenttime

            }
          } // playertrack
        } // miditrack
      } // sequence
    } // for tracks
    MCurrentTime += MSecondsPerSample;
  }

  //----------

  //----------

};

//----------------------------------------------------------------------
#endif
