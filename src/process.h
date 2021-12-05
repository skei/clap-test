#ifndef process_included
#define process_included
//----------------------------------------------------------------------

// in progress!!
// untested..

#define MAX_BLOCK_SIZE 65536


#include "clap/all.h"
#include "audio_file.h"
#include "midi_file.h"
#include "midi_player.h"

//----------------------------------------------------------------------

class Process {

//------------------------------
private:
//------------------------------

  uint32_t              MSamplePos = 0;

//------------------------------
private:
//------------------------------

  clap_process          MContext;
  clap_event_transport  MTransport;
  clap_audio_buffer     MAudioInputs;
  clap_audio_buffer     MAudioOutputs;
  clap_event_list       MEventInputs;
  clap_event_list       MEventOutputs;

  MidiFile              MMidiFile;
  MidiPlayer            MMidiPlayer;

//clap_event            MInputEvent;
//clap_event            MOutputEvent;
  AudioFile             MInputAudio;
  AudioFile             MOutputAudio;

  float                 MAudioInputBuffer1[MAX_BLOCK_SIZE];
  float                 MAudioInputBuffer2[MAX_BLOCK_SIZE];
  float*                MAudioInputBuffers[2] = { MAudioInputBuffer1, MAudioInputBuffer2 };

  float                 MAudioOutputBuffer1[MAX_BLOCK_SIZE];
  float                 MAudioOutputBuffer2[MAX_BLOCK_SIZE];
  float*                MAudioOutputBuffers[2] = { MAudioOutputBuffer1, MAudioOutputBuffer2 };

//------------------------------
public:
//------------------------------

  Process() {
  }

  //----------

  ~Process() {
  }

//------------------------------
public:
//------------------------------

  uint32_t getNumInputEvents() {
    return 0;
  }

  //----------

  const clap_event* getInputEvent(uint32_t index) {
    //clap_event  event;
    //event.type          = CLAP_EVENT_NOTE_ON;
    //event.time          = 0;
    //event.note.channel  = 0;
    //event.note.key      = 0;
    //event.note.velocity = 0.0;
    //return &event;
    return NULL;
  }

  //----------

  void appendInputEvent(const clap_event* event) {
  }

  //----------

  uint32_t getNumOutputEvents() {
    return 0;
  }

  //----------

  const clap_event* getOutputEvent(uint32_t index) {
    return NULL;
  }

  //----------

  void appendOutputEvent(const clap_event* event) {
  }

  //----------

//------------------------------
public: // callbacks
//------------------------------

  static
  uint32_t process_input_events_size(const struct clap_event_list *list) {
    Process* process = (Process*)list->ctx;
    if (process) return process->getNumInputEvents();
    return 0;
  }

  //----------

  static
  const clap_event* process_input_events_get(const struct clap_event_list *list, uint32_t index) {
    Process* process = (Process*)list->ctx;
    if (process) return process->getInputEvent(index);
    return NULL;
  }

  //----------

  static
  void process_input_events_push_back(const struct clap_event_list *list, const clap_event *event) {
    Process* process = (Process*)list->ctx;
    if (process) process->appendInputEvent(event);
  }

  //----------

  static
  uint32_t process_output_events_size(const struct clap_event_list *list) {
    Process* process = (Process*)list->ctx;
    if (process) return process->getNumInputEvents();
    return 0;
  }

  //----------

  static
  const clap_event* process_output_events_get(const struct clap_event_list *list, uint32_t index) {
    Process* process = (Process*)list->ctx;
    if (process) return process->getOutputEvent(index);
    return NULL;
  }

  //----------

  static
  void process_output_events_push_back(const struct clap_event_list *list, const clap_event *event) {
    Process* process = (Process*)list->ctx;
    if (process) process->appendOutputEvent(event);
  }

//------------------------------
public:
//------------------------------

  void prepare_audio_inputs(uint32_t channels, uint32_t latency=0) {
    MAudioInputs.data32           = MAudioInputBuffers;
    MAudioInputs.data64           = NULL;
    MAudioInputs.channel_count    = channels;
    MAudioInputs.latency          = latency;
    MAudioInputs.constant_mask    = 0;
  }

  void prepare_audio_outputs(uint32_t channels, uint32_t latency=0) {
    MAudioOutputs.data32          = MAudioOutputBuffers;
    MAudioOutputs.data64          = NULL;
    MAudioOutputs.channel_count   = channels;
    MAudioOutputs.latency         = latency;
    MAudioOutputs.constant_mask   = 0;
  }

  void prepare_event_inputs() {
    MEventInputs.ctx              = this;//NULL;
    MEventInputs.size             = &process_input_events_size;
    MEventInputs.get              = &process_input_events_get;
    MEventInputs.push_back        = &process_input_events_push_back;
  }

  void prepare_event_outputs() {
    MEventOutputs.ctx             = this;//NULL;
    MEventOutputs.size            = &process_output_events_size;
    MEventOutputs.get             = &process_output_events_get;
    MEventOutputs.push_back       = &process_output_events_push_back;
  }

  void prepare_transport() {
    MTransport.flags              = CLAP_TRANSPORT_IS_PLAYING;
    MTransport.song_pos_beats     = 0;
    MTransport.song_pos_seconds   = 0;
    MTransport.tempo              = 0.0;
    MTransport.tempo_inc          = 0.0;
    MTransport.bar_start          = 0;
    MTransport.bar_number         = 0;
    MTransport.loop_start_beats   = 0;
    MTransport.loop_end_beats     = 0;
    MTransport.loop_start_seconds = 0;
    MTransport.loop_end_seconds   = 0;
    MTransport.tsig_num           = 0;
    MTransport.tsig_denom         = 0;
  }

  void prepare_context(/*uint32_t channels,*/ uint32_t blocksize, uint32_t latency=0) {
    //prepare_audio_inputs(channels,latency,MAudioInputBuffers);
    //prepare_audio_outputs(channels,latency,MAudioInputBuffers);
    //prepare_event_inputs();
    //prepare_event_outputs();
    //prepare_transport();
    MContext.steady_time          = MSamplePos;
    MContext.frames_count         = blocksize;
    MContext.transport            = &MTransport;
    MContext.audio_inputs         = &MAudioInputs;
    MContext.audio_outputs        = &MAudioOutputs;
    MContext.in_events            = &MEventInputs;
    MContext.out_events           = &MEventOutputs;
  }

  //----------

  void process_audio(Instance* instance, arguments_t* arg) {

    if (arg->input_audio) {
      printf("> opening audio input file: %s\n",arg->input_audio);
      bool result = MInputAudio.open(arg->input_audio,AUDIO_FILE_READ);
      if (!result) {
        printf("! couldn't open audio input file\n");
        return;
      }
      printf("> audio input file opened\n");
      printf("    - length: %i\n",(int)MInputAudio.getInfo()->frames);
    }

    if (arg->output_audio) {
      printf("> opening audio output file: %s\n",arg->output_audio);
      bool result = MOutputAudio.open(arg->output_audio,AUDIO_FILE_WRITE,arg->sample_rate,arg->channels);
      if (!result ){
        printf("! couldn't open audio output file\n");
        return;
      }
      printf("> audio output file opened\n");
    }
    else {
      printf("!! no audio output file defined\n");
      return;
    }

    //-----

    prepare_audio_inputs(arg->channels,0);
    prepare_audio_outputs(arg->channels,0);
    prepare_event_inputs();
    prepare_event_outputs();
    prepare_context(arg->block_size,0);
    MSamplePos = 0;

    const clap_plugin* plugin = instance->getClapPlugin();
    uint32_t num_samples = MInputAudio.getInfo()->frames;
    uint32_t num_blocks = num_samples / arg->block_size;
    num_blocks += 1; // just to be sure :-)

    // processing loop

    printf("> processing..\n");
    for (uint32_t i=0; i<num_blocks; i++) {

      if (arg->input_midi) {
        for (int32_t j=0; j<arg->block_size; j++) {
          // prepare events
        }
      }
      else {
        MInputAudio.read(arg->channels,arg->block_size,MAudioInputBuffers);
      }

      MContext.steady_time = MSamplePos;
      MContext.frames_count = arg->block_size;
      plugin->process(plugin,&MContext);
      MOutputAudio.write(arg->channels,arg->block_size, MAudioOutputBuffers);
      MSamplePos += arg->block_size;
    }
    printf("> finished processing\n");

    //-----

    MInputAudio.close();
    printf("> audio input file closed\n");
    MOutputAudio.close();
    printf("> audio output file closed\n");
  }

  //----------

  void process_midi(Instance* instance, arguments_t* arg) {
    printf("PROCESS_MIDI\n");
    ///*int result;
    //result =*/ midifile.load(MArguments.input_midi);
    //MidiSequence* seq = midifile.getMidiSequence();
    //seq->calc_time();
    ////midifile.print();
    //printf("\n");
    //printf("playing midi file\n");
    //printf("\n");
    //midiplayer.initialize(&midifile,44100,0);
    //
    //float midi_length = 60 * 5; // seconds
    //printf("midi length: %.2f\n", midi_length );
    //float num_samples = MArguments.sample_rate * midi_length;
    //printf("num samples: %.2f\n", num_samples );
    //uint32_t num_blocks = num_samples / MArguments.block_size;
    //num_blocks += 1; // just to be sure :-)
    //printf("num blocks: %i\n", num_blocks );
    //for (uint32_t i=0; i<num_blocks; i++) {
    //  // collect midi events
    //  for (int32_t j=0; j<MArguments.block_size; j++) {
    //    midiplayer.process();
    //  }
    //  // render plugin
    //  //plugin->process();
    //  // save output
    //  //audioout.write(MArguments.channels,MArguments.block_size, buffers):
    //}
    ////player.cleanup();
    //printf("finished playing midi file\n");
    //midifile.unload();
    //printf("unloaded midi file\n");
    //printf("\n");
  }

};




#undef MAX_BLOCK_SIZE

//----------------------------------------------------------------------
#endif



























#if 0






//----------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------

clap_process          context;
clap_event_transport  transport;
clap_audio_buffer     audio_inputs;
clap_audio_buffer     audio_outputs;
clap_event_list       event_inputs;
clap_event_list       event_outputs;

//----------------------------------------------------------------------
//
// events
//
//----------------------------------------------------------------------

clap_event input_event;
clap_event output_event;

/*
  called by plugin to find out how many events we have for this block
*/

uint32_t process_input_events_size(const struct clap_event_list *list) {
  return 0;
}

//----------

/*
  plugin want specified event
*/

const clap_event* process_input_events_get(const struct clap_event_list *list, uint32_t index) {
  input_event.type          = CLAP_EVENT_NOTE_ON;
  input_event.time          = 0;
  input_event.note.channel  = 0;
  input_event.note.key      = 0;
  input_event.note.velocity = 0.0;
  return &input_event;
}

//----------

/*
  ???
  plugin want to write input event?
  send event to next plugin in chain?
*/

void process_input_events_push_back(const struct clap_event_list *list, const clap_event *event) {
}

//------------------------------
//
//------------------------------

/*
  we ignore output events..
*/

//----------

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

//void process(const clap_plugin* plugin) {
//
//  clap_process          context;
//  clap_event_transport  transport;
//  clap_audio_buffer     audio_inputs;
//  clap_audio_buffer     audio_outputs;
//  clap_event_list       event_inputs;
//  clap_event_list       event_outputs;
//
//  //-----
//
//  transport.flags               = CLAP_TRANSPORT_IS_PLAYING;
//  transport.song_pos_beats      = 0;
//  transport.song_pos_seconds    = 0;
//  transport.tempo               = 0.0;
//  transport.tempo_inc           = 0.0;
//  transport.bar_start           = 0;
//  transport.bar_number          = 0;
//  transport.loop_start_beats    = 0;
//  transport.loop_end_beats      = 0;
//  transport.loop_start_seconds  = 0;
//  transport.loop_end_seconds    = 0;
//  transport.tsig_num            = 0;
//  transport.tsig_denom          = 0;
//
//  audio_inputs.data32           = NULL;
//  audio_inputs.data64           = NULL;
//  audio_inputs.channel_count    = 0;
//  audio_inputs.latency          = 0;
//  audio_inputs.constant_mask    = 0;
//
//  audio_outputs.data32          = NULL;
//  audio_outputs.data64          = NULL;
//  audio_outputs.channel_count   = 0;
//  audio_outputs.latency         = 0;
//  audio_outputs.constant_mask   = 0;
//
//  event_inputs.ctx              = NULL;
//  event_inputs.size             = &process_input_events_size;
//  event_inputs.get              = &process_input_events_get;
//  event_inputs.push_back        = &process_input_events_push_back;
//
//  event_outputs.ctx             = NULL;
//  event_outputs.size            = &process_output_events_size;
//  event_outputs.get             = &process_output_events_get;
//  event_outputs.push_back       = &process_output_events_push_back;
//
//  context.steady_time           = 0;
//  context.frames_count          = 0;
//  context.transport             = &transport;
//  context.audio_inputs          = &audio_inputs;
//  context.audio_outputs         = &audio_outputs;
//  context.in_events             = &event_inputs;
//  context.out_events            = &event_outputs;
//
//  //clap_process_status status =
//  plugin->process(plugin,&context);
//  //while (plugin->process(plugin,&context) == CLAP_PROCESS_CONTINUE) {
//  //}
//
//}

//----------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------

AudioFile     in_audio;
AudioFile     out_audio;

float         buffer1[MAX_BLOCK_SIZE];
float         buffer2[MAX_BLOCK_SIZE];
float*        buffers[2] = { buffer1, buffer2 };

//----------

void process_audio(Instance* instance, arguments_t* arg) {

  printf("> opening audio input file: %s\n",arg->input_audio);
  bool result = in_audio.open(arg->input_audio);
  if (!result) {
    printf("! couldn't open audio input file\n");
    return;
  }
  printf("> audio input file opened\n");

  printf("> opening audio output file: %s\n",arg->output_audio);
  result = out_audio.open(arg->output_audio,AUDIO_FILE_WRITE,arg->sample_rate,arg->channels);
  if (!result ){
    printf("! couldn't open audio output file\n");
    return;
  }
  printf("> audio output file opened\n");

  printf("> GO!\n");

  uint32_t num_samples = in_audio.getInfo()->frames;
  uint32_t num_blocks = num_samples / arg->block_size;
  num_blocks += 1; // just to be sure :-)

  for (uint32_t i=0; i<num_blocks; i++) {
    //for (int32_t j=0; j<arg->block_size; j++) {}
    in_audio.read(arg->channels,arg->block_size,buffers);
    //plugin->process(plugin,&context);
    out_audio.write(arg->channels,arg->block_size, buffers);
  }

  in_audio.close();
  printf("> audio input file closed\n");
  out_audio.close();
  printf("> audio output file closed\n");

}

//----------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------

MidiFile      midifile;
MidiPlayer    midiplayer;

void process_midi(Instance* instance, arguments_t* arg) {
//  /*int result;
//  result =*/ midifile.load(MArguments.input_midi);
//  MidiSequence* seq = midifile.getMidiSequence();
//  seq->calc_time();
//  //midifile.print();
//  printf("\n");
//  printf("playing midi file\n");
//  printf("\n");
//  midiplayer.initialize(&midifile,44100,0);
//
//  float midi_length = 60 * 5; // seconds
//  printf("midi length: %.2f\n", midi_length );
//  float num_samples = MArguments.sample_rate * midi_length;
//  printf("num samples: %.2f\n", num_samples );
//  uint32_t num_blocks = num_samples / MArguments.block_size;
//  num_blocks += 1; // just to be sure :-)
//  printf("num blocks: %i\n", num_blocks );
//  for (uint32_t i=0; i<num_blocks; i++) {
//    // collect midi events
//    for (int32_t j=0; j<MArguments.block_size; j++) {
//      midiplayer.process();
//    }
//    // render plugin
//    //plugin->process();
//    // save output
//    //audioout.write(MArguments.channels,MArguments.block_size, buffers):
//  }
//  //player.cleanup();
//  printf("finished playing midi file\n");
//  midifile.unload();
//  printf("unloaded midi file\n");
//  printf("\n");
}








#endif // 0







