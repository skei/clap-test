#ifndef process_included
#define process_included
//----------------------------------------------------------------------

#include "clap/all.h"

//----------------------------------------------------------------------

// global variables!

clap_event input_event;
clap_event output_event;

//----------------------------------------------------------------------
//
// events
//
//----------------------------------------------------------------------

uint32_t process_input_events_size(const struct clap_event_list *list) {
  return 0;
}

//----------

const clap_event* process_input_events_get(const struct clap_event_list *list, uint32_t index) {
  input_event.type          = CLAP_EVENT_NOTE_ON;
  input_event.time          = 0;
  input_event.note.channel  = 0;
  input_event.note.key      = 0;
  input_event.note.velocity = 0.0;
  return &input_event;
}

//----------

void process_input_events_push_back(const struct clap_event_list *list, const clap_event *event) {
}

//------------------------------
//
//------------------------------

uint32_t process_output_events_size(const struct clap_event_list *list) {
  return 0;
}

//----------

const clap_event* process_output_events_get(const struct clap_event_list *list, uint32_t index) {
  return NULL;
}

//----------

void process_output_events_push_back(const struct clap_event_list *list, const clap_event *event) {
}

//----------------------------------------------------------------------
//
// process
//
//----------------------------------------------------------------------

void process_audio(const clap_plugin* plugin) {

  clap_process          context;
  clap_event_transport  transport;
  clap_audio_buffer     audio_inputs;
  clap_audio_buffer     audio_outputs;
  clap_event_list       event_inputs;
  clap_event_list       event_outputs;

  //-----

  transport.flags               = CLAP_TRANSPORT_IS_PLAYING;
  transport.song_pos_beats      = 0;
  transport.song_pos_seconds    = 0;
  transport.tempo               = 0.0;
  transport.tempo_inc           = 0.0;
  transport.bar_start           = 0;
  transport.bar_number          = 0;
  transport.loop_start_beats    = 0;
  transport.loop_end_beats      = 0;
  transport.loop_start_seconds  = 0;
  transport.loop_end_seconds    = 0;
  transport.tsig_num            = 0;
  transport.tsig_denom          = 0;

  audio_inputs.data32           = NULL;
  audio_inputs.data64           = NULL;
  audio_inputs.channel_count    = 0;
  audio_inputs.latency          = 0;
  audio_inputs.constant_mask    = 0;

  audio_outputs.data32          = NULL;
  audio_outputs.data64          = NULL;
  audio_outputs.channel_count   = 0;
  audio_outputs.latency         = 0;
  audio_outputs.constant_mask   = 0;

  event_inputs.ctx              = NULL;
  event_inputs.size             = &process_input_events_size;
  event_inputs.get              = &process_input_events_get;
  event_inputs.push_back        = &process_input_events_push_back;

  event_outputs.ctx             = NULL;
  event_outputs.size            = &process_output_events_size;
  event_outputs.get             = &process_output_events_get;
  event_outputs.push_back       = &process_output_events_push_back;

  context.steady_time           = 0;
  context.frames_count          = 0;
  context.transport             = &transport;
  context.audio_inputs          = &audio_inputs;
  context.audio_outputs         = &audio_outputs;
  context.in_events             = &event_inputs;
  context.out_events            = &event_outputs;

  //clap_process_status status =
  plugin->process(plugin,&context);
  //while (plugin->process(plugin,&context) == CLAP_PROCESS_CONTINUE) {
  //}

}

//----------------------------------------------------------------------
#endif
