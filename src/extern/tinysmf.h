#include <stdio.h>
#include <stdint.h>

#pragma once

//----------

/**
* file info
*/

typedef enum {
  SMF_ONE_TRACK     = 0,
  SMF_MANY_TRACKS   = 1,
  SMF_MANY_PATTERNS = 2
} tinysmf_file_format_t;

typedef enum {
  SMF_PPQN,
  SMF_SMPTE
} tinysmf_division_type_t;

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

/**
* events
*/

typedef enum {
  TINYSMF_MIDI_EVENT,
  TINYSMF_SYSEX_EVENT,
  TINYSMF_META_EVENT
} tinysmf_event_type_t;

#define TINYSMF_EVENT_FIELDS          \
  tinysmf_event_type_t event_type;    \
  uint32_t delta

struct tinysmf_event {
  TINYSMF_EVENT_FIELDS;
};

struct tinysmf_midi_event {
  TINYSMF_EVENT_FIELDS;
  uint8_t bytes[4];
};

typedef enum {
  TINYSMF_META_TYPE_SEQUENCE_NUMBER  = 0x00,
  TINYSMF_META_TYPE_TEXT_EVENT       = 0x01,
  TINYSMF_META_TYPE_COPYRIGHT_NOTICE = 0x02,
  TINYSMF_META_TYPE_TRACK_NAME       = 0x03,
  TINYSMF_META_TYPE_INSTRUMENT_NAME  = 0x04,
  TINYSMF_META_TYPE_LYRIC            = 0x05,
  TINYSMF_META_TYPE_MARKER           = 0x06,
  TINYSMF_META_TYPE_CUE_POINT        = 0x07,
  TINYSMF_META_TYPE_MIDI_CHANNEL     = 0x20,
  TINYSMF_META_TYPE_END_OF_TRACK     = 0x2F,
  TINYSMF_META_TYPE_SET_TEMPO        = 0x51,
  TINYSMF_META_TYPE_SMPTE_OFFSET     = 0x54,
  TINYSMF_META_TYPE_TIME_SIGNATURE   = 0x58,
  TINYSMF_META_TYPE_KEY_SIGNATURE    = 0x59,
  /* you're on your own, chief */
  TINYSMF_META_TYPE_SEQ_SPECIFIC     = 0x7F
} tinysmf_meta_event_type_t;

struct tinysmf_meta_event {
  TINYSMF_EVENT_FIELDS;
  tinysmf_meta_event_type_t meta_type;
  size_t nbytes;
  union {
    int midi_channel;
    double bpm;
  } cooked;
  uint8_t raw[];
};

//----------

/**
* public API
*/

typedef enum {
  TINYSMF_PARSE_CHUNK = 0,
  TINYSMF_SKIP_CHUNK  = 1
} tinysmf_chunk_cb_ret_t;

struct tinysmf_parser_ctx;

typedef void (*tinysmf_file_info_cb_t)(struct tinysmf_parser_ctx *);
typedef tinysmf_chunk_cb_ret_t (*tinysmf_track_cb_t)(struct tinysmf_parser_ctx *, int track_idx);
typedef void (*tinysmf_meta_event_cb_t)(struct tinysmf_parser_ctx *, const struct tinysmf_meta_event *);
typedef void (*tinysmf_midi_event_cb_t)(struct tinysmf_parser_ctx *, const struct tinysmf_midi_event *);

struct tinysmf_parser_ctx {
  /************************************************************************
  * PUBLIC
  *
  * set these callback values to hook into your client code
  ************************************************************************/
  tinysmf_file_info_cb_t file_info_cb;
  /* your callback should return TINYSMF_PARSE_CHUNK if you want to
  * receive callbacks for events on this track, or TINYSMF_SKIP_CHUNK
  * if you don't.
  *
  * tinysmf ignores the return value of track_end_cb. */
  tinysmf_track_cb_t track_start_cb;
  tinysmf_track_cb_t track_end_cb;
  tinysmf_meta_event_cb_t meta_event_cb;
  tinysmf_midi_event_cb_t midi_event_cb;
  /************************************************************************
  * READ-ONLY
  ************************************************************************/
  const struct tinysmf_file_info file_info;
};

int tinysmf_parse_stream(struct tinysmf_parser_ctx *, FILE *);
