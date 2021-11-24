#ifndef mid_included
#define mid_included
//----------------------------------------------------------------------

// TODO: class-ify

#include "extern/tinysmf.h"
#include "extern/tinysmf.c"

#include <math.h>

//----------------------------------------------------------------------

struct midi_event_t {
//uint32_t  time;
  double  time;
  uint8_t data[4];
};

//----------

struct midi_track_t {
  const char*         name;
  uint32_t            num_events;
  midi_event_t**      events;
  //midi_event_array_t  events;
};

//----------

struct midi_seq_t {
  uint32_t        num_tracks;
  midi_track_t**  tracks;
};

//----------

struct midi_parser_t {
  //struct uhe_test_host *host;
  tinysmf_parser_ctx  ctx;
  int                 time;
  double              bpm;
  midi_seq_t          mid;
};

//----------------------------------------------------------------------
// tinysmf
//----------------------------------------------------------------------

//static
void on_meta_event(struct tinysmf_parser_ctx *ctx, const struct tinysmf_meta_event *ev) {
  midi_parser_t *p = (midi_parser_t *)ctx;
  switch (ev->meta_type) {
    case TINYSMF_META_TYPE_SET_TEMPO:
      p->bpm = ev->cooked.bpm;
      break;
    default:
      break;
  }
}

//----------

//static
void on_midi_event(struct tinysmf_parser_ctx *ctx, const struct tinysmf_midi_event *ev) {
  midi_parser_t *p = (midi_parser_t *)ctx;

  midi_event_t* e = (midi_event_t*)malloc(sizeof(midi_event_t));
  e->data[0] = ev->bytes[0];
  e->data[1] = ev->bytes[1];
  e->data[2] = ev->bytes[2];
  e->data[3] = ev->bytes[3];
  p->time += ev->delta;
  e->time = (60.0 / p->bpm) * (p->time / (double) p->ctx.file_info.division.ppqn);

//  p->mid.appendEvent(e);

//VECTOR_PUSH_BACK(&p->host->sequence, e);

}

//----------

//static
tinysmf_chunk_cb_ret_t on_track_start(struct tinysmf_parser_ctx *ctx, int track_idx) {
  midi_parser_t *p = (midi_parser_t *)ctx;
  p->time = 0;
  return (track_idx > 1) ? TINYSMF_SKIP_CHUNK : TINYSMF_PARSE_CHUNK;
}

//----------

//static
int midi_note_cmp(const void *a, const void *b) {
  const midi_event_t* na;
  const midi_event_t* nb;
  na = (const midi_event_t*)a;
  nb = (const midi_event_t*)b;
  double diff = na->time - nb->time;
  /* bit this idea from JUCE: sorts the MIDI notes so that a note-offs
   * happen before note-ons if they have the same timestamp. */
  if (diff) return lrint(copysign(1.0, diff));
  if (na->data[0] == 0x80 && nb->data[0] == 0x90) return -1;
  if (na->data[0] == 0x90 && nb->data[0] == 0x80) return 1;
  return 0;
}

//----------------------------------------------------------------------
// midi seq
//----------------------------------------------------------------------

void init_midi_seq(midi_seq_t* seq) {
  seq->num_tracks = 0;
  seq->tracks       = NULL;
}

//----------


int load_midi_seq(midi_seq_t* seq, const char* path) {
  struct midi_parser_t p = {};
  FILE *f;
  p.time = 0;
  p.bpm = 0.0;
  p.ctx.track_start_cb = on_track_start;
  p.ctx.midi_event_cb  = on_midi_event;
  p.ctx.meta_event_cb  = on_meta_event;
  if (!(f = fopen(path, "rb"))) {
    perror("couldn't open MIDI file");
    goto err_fopen;
  }
  if (tinysmf_parse_stream((tinysmf_parser_ctx*)&p, f)) {
    fprintf(stderr, "error parsing \"%s\"\n", path);
    goto err_parse;
  }

//qsort(sequence.data, sequence.size, sizeof(*sequence.data), midi_note_cmp);

  fclose(f);
  return 0;
err_parse:
  fclose(f);
err_fopen:
  return -1;

}

//----------

void append_track(midi_seq_t* seq, midi_track_t* track) {
  uint32_t index;
  index = seq->num_tracks;
  seq->tracks[index] = track;
  seq->num_tracks += 1;
}

//----------

void delete_tracks(midi_seq_t* seq) {
  uint32_t i;
  for (i=0; i<seq->num_tracks; i++)
    free(seq->tracks[i]);
}

//----------------------------------------------------------------------
// midi track
//----------------------------------------------------------------------

void init_midi_track(midi_track_t* track) {
  track->num_events = 0;
  track->events     = NULL;
}

//----------

void append_event(midi_track_t* track, midi_event_t* event) {
  uint32_t index;
  index = track->num_events;
  track->events[index] = event;
  track->num_events += 1;
}

//----------

midi_event_t* new_event() {
  midi_event_t* event = (midi_event_t*)malloc(sizeof(midi_event_t));
  return event;
}

//----------

void delete_events(midi_track_t* track) {
  uint32_t i;
  for (i=0; i<track->num_events; i++) free(track->events[i]);
}

//----------------------------------------------------------------------
// event track
//----------------------------------------------------------------------

//void init_midi_event(midi_event_t* event) {
//  event->time       = 0.0;
//  event->data[0]    = 0;
//  event->data[1]    = 0;
//  event->data[2]    = 0;
//  event->data[3]    = 0;
//}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------



//----------------------------------------------------------------------
#endif
