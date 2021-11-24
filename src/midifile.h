#ifndef midifile_included
#define midifile_included
//----------------------------------------------------------------------

#include "extern/tinysmf.h"
#include "extern/tinysmf.c"

#include <math.h>
#include <vector>

//----------------------------------------------------------------------

//class MidiSequence;

//----------

struct MidiEvent {
  uint32_t  time    = 0;
  uint8_t   data[4] = {0};
};

//----------

struct MidiParser {
  tinysmf_parser_ctx  ctx   = {};
  int                 time  = 0;
  double              bpm   = 0.0;
  class MidiSequence* seq   = NULL;
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

  static
  void on_midi_event(struct tinysmf_parser_ctx *ctx, const struct tinysmf_midi_event *ev) {
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
    parser->seq->appendEvent(event);
  }

  //----------

  /*
    paste everything into one 'track'?
  */

  static
  tinysmf_chunk_cb_ret_t on_track_start(struct tinysmf_parser_ctx *ctx, int track_idx) {
    MidiParser *parser = (MidiParser *)ctx;
    parser->time = 0;
    return (track_idx > 1) ? TINYSMF_SKIP_CHUNK : TINYSMF_PARSE_CHUNK;
  }

  //----------

  static
  void on_meta_event(struct tinysmf_parser_ctx *ctx, const struct tinysmf_meta_event *ev) {
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

  static
  int midi_note_cmp(const void *a, const void *b) {
    const MidiEvent* na = (const MidiEvent*)a;
    const MidiEvent* nb = (const MidiEvent*)b;
    double diff = na->time - nb->time;
    if (diff) return lrint(copysign(1.0, diff));
    if (na->data[0] == 0x80 && nb->data[0] == 0x90) return -1;
    if (na->data[0] == 0x90 && nb->data[0] == 0x80) return 1;
    return 0;
  }

//------------------------------
public:
//------------------------------

  int load(const char* path) {
    MidiParser parser;
    parser.time = 0;
    parser.bpm = 0.0;
    parser.seq = this;
    parser.ctx.track_start_cb = on_track_start;
    parser.ctx.midi_event_cb  = on_midi_event;
    parser.ctx.meta_event_cb  = on_meta_event;
    FILE *fp;
    if (!(fp = fopen(path, "rb"))) {
      perror("couldn't open MIDI file");
      return -1;
    }
    if (tinysmf_parse_stream((tinysmf_parser_ctx*)&parser, fp)) {
      fprintf(stderr, "error parsing \"%s\"\n", path);
      fclose(fp);
      return -1;
    }

    // MEvents[0]
    // &MEvents[0];
    // MEvents.data()
    qsort( MEvents.data(), MEvents.size(), sizeof(MidiEvent*), midi_note_cmp);

    fclose(fp);
    return 0;
  }

};

//----------------------------------------------------------------------
#endif
