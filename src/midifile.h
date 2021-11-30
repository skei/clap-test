#ifndef midifile_included
#define midifile_included
//----------------------------------------------------------------------

/*
  this is too weird
  see midi_file.h instead
*/

#if 0

#include "extern/tinysmf.h"
#include "extern/tinysmf.c"

#include <math.h>
#include <vector>

//----------------------------------------------------------------------

//class MidiSequence;

//----------

struct MidiEvent {
  //uint32_t  time    = 0;
  float     time    = 0;
  uint8_t   data[4] = {0};
};

//----------

struct MidiParser {
  tinysmf_parser_ctx  ctx   = {};
  class MidiSequence* seq   = NULL;
  int                 time  = 0;
  double              bpm   = 0.0;
};

//----------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------

class MidiSequence {

//------------------------------
private:
//------------------------------

  //Array<MidiTrack*> MTracks;
  std::vector<MidiEvent*>  MEvents;

//------------------------------
public:
//------------------------------

  uint32_t    getNumEvents()  { return MEvents.size(); }
  MidiEvent*  getEvent(int i) { return MEvents[i]; }
  MidiEvent** getEvents()     { return &MEvents[0]; }

  //----------

  void appendEvent(MidiEvent* AEvent) {
    MEvents.push_back(AEvent);
  }

  //----------

  void deleteEvents() {
    for (uint32_t i=0; i<MEvents.size(); i++) delete MEvents[i];
  }

//------------------------------
private:
//------------------------------

  /*
    ouch.. bpm is 0 during the first incoming midi events ???
  */

  static
  void on_midi_event(struct tinysmf_parser_ctx *ctx, const struct tinysmf_midi_event *ev) {
    //printf("on_midi_event\n");
    MidiParser* parser = (MidiParser *)ctx;
    MidiEvent* event = new MidiEvent();
    event->data[0] = ev->bytes[0];
    event->data[1] = ev->bytes[1];
    event->data[2] = ev->bytes[2];
    event->data[3] = ev->bytes[3];
    parser->time += ev->delta;
    event->time = (60.0 / parser->bpm) * (parser->time / (double)parser->ctx.file_info.division.ppqn);
    //p->mid.appendEvent(e);
    //VECTOR_PUSH_BACK(&p->host->sequence, e);

    printf("parser->bpm %.3f ctx.ppqn %i\n",parser->bpm,parser->ctx.file_info.division.ppqn);
    printf("parser->time %i -> event->time %.3f\n",parser->time,event->time);

    parser->seq->appendEvent(event);
  }

  //----------

  /*
    paste everything into one 'track'?
  */

  static
  tinysmf_chunk_cb_ret_t on_track_start(struct tinysmf_parser_ctx *ctx, int track_idx) {
    //printf("on_track_start\n");
    MidiParser* parser = (MidiParser*)ctx;
    parser->time = 0;
    //return TINYSMF_PARSE_CHUNK;
    return (track_idx > 1) ? TINYSMF_SKIP_CHUNK : TINYSMF_PARSE_CHUNK;
  }

  //----------

  static
  void on_meta_event(struct tinysmf_parser_ctx *ctx, const struct tinysmf_meta_event *ev) {
    //printf("on_meta_event\n");
    MidiParser *p = (MidiParser *)ctx;
    switch (ev->meta_type) {
      case TINYSMF_META_TYPE_SET_TEMPO:
        p->bpm = ev->cooked.bpm;
        break;
      default:
        break;
    }
  }

  //----------

  /*
    < 0	  The element pointed to by p1 goes before the element pointed to by p2
    0	    The element pointed to by p1 is equivalent to the element pointed to by p2
    > 0	  The element pointed to by p1 goes after the element pointed to by p2
  */


  static
  int midi_note_cmp(const void *a, const void *b) {
    const MidiEvent* na = (const MidiEvent*)a;
    const MidiEvent* nb = (const MidiEvent*)b;

    printf("a %.3f b %.3f\n",na->time,nb->time);

    if (na->time < nb->time) return -1;
    if (na->time > nb->time) return  1;

//    double diff = na->time - nb->time;
//    if (diff) return lrint(copysign(1.0, diff));
//    if (na->data[0] == 0x80 && nb->data[0] == 0x90) return -1;
//    if (na->data[0] == 0x90 && nb->data[0] == 0x80) return 1;

    return 0;
  }

//------------------------------
public:
//------------------------------

  int load(const char* path) {
    printf("loading: %s\n",path);

    MidiParser parser;
    parser.ctx.track_start_cb = on_track_start;
    parser.ctx.midi_event_cb  = on_midi_event;
    parser.ctx.meta_event_cb  = on_meta_event;
    parser.time = 0;
    parser.bpm = 120.0;//0.0;
    parser.seq = this;

    FILE *fp;
    if (!(fp = fopen(path, "rb"))) {
      perror("couldn't open MIDI file");
      return -1;
    }
    if (tinysmf_parse_stream((tinysmf_parser_ctx*)&parser, fp)) {
    //if (tinysmf_parse_stream(&parser.ctx,fp)) {
      fprintf(stderr, "error parsing \"%s\"\n", path);
      fclose(fp);
      return -1;
    }

//    int num_events = parser.seq->getNumEvents();//  MEvents.size();
//    printf("MEvents.size(): %i\n", (int)MEvents.size());
//    printf("parser.seq->getNumEvents(): %i\n", parser.seq->getNumEvents());

    printf("parser.bpm: %f\n",parser.bpm);

    /*
    struct tinysmf_file_info {
      tinysmf_file_format_t format;
      int num_tracks;
      tinysmf_division_type_t division_type;
      union {
        int ppqn;
        struct {
          int format;
          int ticks;
        } smpte;
      } division;
    };
    */

    //printf("ctx.file_info.format:         %i\n",parser.ctx.file_info.format);
    //printf("ctx.file_info.num_tracks:     %i\n",parser.ctx.file_info.num_tracks);
    //printf("ctx.file_info.division_type:  %i\n",parser.ctx.file_info.division_type);
    //printf("ctx.file_info.division.ppqn:  %i\n",parser.ctx.file_info.division.ppqn);

    // MEvents[0]
    // &MEvents[0];
    // MEvents.data()

    // is this correct?
    qsort( &MEvents[0], MEvents.size(), sizeof(MidiEvent*), midi_note_cmp);

    fclose(fp);
    return 0;
  }

};

#endif // 0

//----------------------------------------------------------------------
#endif
