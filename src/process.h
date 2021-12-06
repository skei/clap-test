#ifndef process_included
#define process_included
//----------------------------------------------------------------------

// in progress!!
// untested..

#define MAX_BLOCK_EVENTS  1024
#define MAX_BLOCK_SIZE    65536


#include "clap/all.h"
#include "audio_file.h"
#include "midi_file.h"
#include "midi_player.h"

typedef std::vector<clap_event*> clap_events;

//----------------------------------------------------------------------

class Process {

//------------------------------
private:
//------------------------------

  clap_process          MClapContext;
  clap_event_transport  MClapTransport;
  clap_audio_buffer     MClapAudioInputs;
  clap_audio_buffer     MClapAudioOutputs;
  clap_event_list       MClapEventInputs;
  clap_event_list       MClapEventOutputs;

  AudioFile             MInputAudioFile;
  AudioFile             MOutputAudioFile;
  float                 MAudioInputBuffer1[MAX_BLOCK_SIZE];
  float                 MAudioInputBuffer2[MAX_BLOCK_SIZE];
  float*                MAudioInputBuffers[2] = { MAudioInputBuffer1, MAudioInputBuffer2 };
  float                 MAudioOutputBuffer1[MAX_BLOCK_SIZE];
  float                 MAudioOutputBuffer2[MAX_BLOCK_SIZE];
  float*                MAudioOutputBuffers[2] = { MAudioOutputBuffer1, MAudioOutputBuffer2 };

  MidiFile              MMidiFile;
  MidiPlayer            MMidiPlayer;
  MidiSequence*         MMidiSequence;
  MidiEvents            MBlockMidiInputEvents;
  clap_events           MBlockClapInputEvents;

  Instance*             MInstance       = NULL;
  float                 MSampleRate     = 0.0;
  uint32_t              MCurrentSample  = 0;
  float                 MCurrentTime    = 0.0;

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

  uint32_t input_events_size() {
    return MBlockMidiInputEvents.size();
  }

  //----------

  const clap_event* input_events_get(uint32_t index) {
    MidiEvent* midievent = MBlockMidiInputEvents[index];
    float   time    = midievent->time - MCurrentTime;
    uint8_t msg1    = midievent->msg1;
    uint8_t msg2    = midievent->msg2;
    uint8_t msg3    = midievent->msg3;
    int32_t offset  = floorf(time * MSampleRate);
    printf("  process.h / getInputEvent: offset %i : %02x %02x %02x\n",offset,msg1,msg2,msg3);
    clap_event* event = (clap_event*)malloc(sizeof(clap_event));  // deleted in deleteInputEvents()
    MBlockClapInputEvents.push_back(event);
    memset(event,0,sizeof(clap_event));
    event->type = CLAP_EVENT_MIDI;
    event->time = offset;
    event->midi.data[0] = 1;//msg1; ..... these doesn't survive
    event->midi.data[1] = 2;//msg2;       (0 in plugin)..
    event->midi.data[2] = 3;//msg3;
    return event;
  }

  //----------

  void input_events_push_back(const clap_event* event) {
  }

  //----------

  uint32_t output_events_size() {
    return 0;
  }

  //----------

  const clap_event* output_events_get(uint32_t index) {
    return NULL;
  }

  //----------

  void output_events_push_back(const clap_event* event) {
  }

  //----------

//------------------------------
public: // callbacks
//------------------------------

  static
  uint32_t process_input_events_size(const struct clap_event_list *list) {
    Process* process = (Process*)list->ctx;
    if (process) return process->input_events_size();
    return 0;
  }

  //----------

  static
  const clap_event* process_input_events_get(const struct clap_event_list *list, uint32_t index) {
    Process* process = (Process*)list->ctx;
    if (process) return process->input_events_get(index);
    return NULL;
  }

  //----------

  static
  void process_input_events_push_back(const struct clap_event_list *list, const clap_event *event) {
    Process* process = (Process*)list->ctx;
    if (process) process->input_events_push_back(event);
  }

  //----------

  static
  uint32_t process_output_events_size(const struct clap_event_list *list) {
    Process* process = (Process*)list->ctx;
    if (process) return process->output_events_size();
    return 0;
  }

  //----------

  static
  const clap_event* process_output_events_get(const struct clap_event_list *list, uint32_t index) {
    Process* process = (Process*)list->ctx;
    if (process) return process->output_events_get(index);
    return NULL;
  }

  //----------

  static
  void process_output_events_push_back(const struct clap_event_list *list, const clap_event *event) {
    Process* process = (Process*)list->ctx;
    if (process) process->output_events_push_back(event);
  }

//------------------------------
public:
//------------------------------

  void prepare_audio_inputs(uint32_t channels, uint32_t latency=0) {
    MClapAudioInputs.data32           = MAudioInputBuffers;
    MClapAudioInputs.data64           = NULL;
    MClapAudioInputs.channel_count    = channels;
    MClapAudioInputs.latency          = latency;
    MClapAudioInputs.constant_mask    = 0;
  }

  void prepare_audio_outputs(uint32_t channels, uint32_t latency=0) {
    MClapAudioOutputs.data32          = MAudioOutputBuffers;
    MClapAudioOutputs.data64          = NULL;
    MClapAudioOutputs.channel_count   = channels;
    MClapAudioOutputs.latency         = latency;
    MClapAudioOutputs.constant_mask   = 0;
  }

  void prepare_event_inputs() {
    MClapEventInputs.ctx              = this;//NULL;
    MClapEventInputs.size             = &process_input_events_size;
    MClapEventInputs.get              = &process_input_events_get;
    MClapEventInputs.push_back        = &process_input_events_push_back;
  }

  void prepare_event_outputs() {
    MClapEventOutputs.ctx             = this;//NULL;
    MClapEventOutputs.size            = &process_output_events_size;
    MClapEventOutputs.get             = &process_output_events_get;
    MClapEventOutputs.push_back       = &process_output_events_push_back;
  }

  void prepare_transport() {
    MClapTransport.flags              = CLAP_TRANSPORT_IS_PLAYING;
    MClapTransport.song_pos_beats     = 0;
    MClapTransport.song_pos_seconds   = 0;
    MClapTransport.tempo              = 0.0;
    MClapTransport.tempo_inc          = 0.0;
    MClapTransport.bar_start          = 0;
    MClapTransport.bar_number         = 0;
    MClapTransport.loop_start_beats   = 0;
    MClapTransport.loop_end_beats     = 0;
    MClapTransport.loop_start_seconds = 0;
    MClapTransport.loop_end_seconds   = 0;
    MClapTransport.tsig_num           = 0;
    MClapTransport.tsig_denom         = 0;
  }

  void prepare_context(/*uint32_t channels,*/ uint32_t blocksize, uint32_t latency=0) {
    MClapContext.steady_time          = MCurrentSample;
    MClapContext.frames_count         = blocksize;
    MClapContext.transport            = &MClapTransport;
    MClapContext.audio_inputs         = &MClapAudioInputs;
    MClapContext.audio_outputs        = &MClapAudioOutputs;
    MClapContext.in_events            = &MClapEventInputs;
    MClapContext.out_events           = &MClapEventOutputs;
  }

//------------------------------
private:
//------------------------------

  /*
    start with 0 events for current blocks
  */

  const clap_event* getInputEvent(uint32_t AIndex) {
    return NULL;
  }

  //----------

  void clearInputEvents() {
    MBlockMidiInputEvents.clear();
    MBlockClapInputEvents.clear();
  }

  //----------

  /*
    delete events we potentially added
    - MidiPlayer.GetEventsForBlock()
    - Process.input_events_get()
  */

  void deleteInputEvents() {
    for (uint32_t i=0; i<MBlockClapInputEvents.size(); i++) {
      free( MBlockClapInputEvents[i] );
    }
    MBlockClapInputEvents.clear();
  }

//------------------------------
public:
//------------------------------

  void process(Instance* instance, arguments_t* arg) {

    MInstance = instance;
    MSampleRate = arg->sample_rate;
    const clap_plugin* plugin = instance->getClapPlugin();
    uint32_t num_samples = 0;

    // midi input

    if (arg->input_midi) {
      /*int result =*/ MMidiFile.load(arg->input_midi);
      //MidiSequence* seq = MMidiFile.getMidiSequence();
      //seq->calc_time();
      MMidiPlayer.initialize(&MMidiFile,arg->sample_rate);
      MMidiSequence = MMidiPlayer.getMidiSequence();
      float midi_length = MMidiSequence->length;
      num_samples = arg->sample_rate * midi_length;
    }

    // audio input

    if (arg->input_audio) {
      printf("> opening audio input file: %s\n",arg->input_audio);
      bool result = MInputAudioFile.open(arg->input_audio,AUDIO_FILE_READ);
      if (!result) {
        printf("! couldn't open audio input file\n");
        return;
      }
      printf("> audio input file opened\n");
      printf("    - length: %i\n",(int)MInputAudioFile.getInfo()->frames);
      num_samples = MInputAudioFile.getInfo()->frames;
    }

    // audio output

    if (arg->output_audio) {
      printf("> opening audio output file: %s\n",arg->output_audio);
      bool result = MOutputAudioFile.open(arg->output_audio,AUDIO_FILE_WRITE,arg->sample_rate,arg->channels);
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

    if (num_samples == 0) {
      printf("!! num_samples == 0\n");
      //return;
    }

    if (num_samples >= (arg->sample_rate * 60.0)) {
      printf("! 1 minute.. truncation\n" );
      num_samples = 60.0 * arg->sample_rate;
    }

    uint32_t num_blocks = num_samples / arg->block_size;
    num_blocks += 1; // just to be sure :-)
    float seconds_per_block = arg->block_size / arg->sample_rate;

    // prepare

    prepare_audio_inputs(arg->channels,0);
    prepare_audio_outputs(arg->channels,0);
    prepare_event_inputs();
    prepare_event_outputs();
    prepare_context(arg->block_size,0);

    MCurrentSample  = 0;    // current position (in samples)
    MCurrentTime    = 0.0;  // current position (in seconds)

    // process all blocks

    printf("> processing %i blocks\n",num_blocks);
    for (uint32_t i=0; i<num_blocks; i++) {
      //printf("block %i\n",i);

      // events for current block
      if (arg->input_midi) {
        clearInputEvents();
        MMidiPlayer.GetEventsForBlock(MCurrentTime,seconds_per_block,&MBlockMidiInputEvents);
      }
      else {
        MInputAudioFile.read(arg->channels,arg->block_size,MAudioInputBuffers);
      }

      // update current time
      MClapContext.steady_time = MCurrentSample;
      MClapContext.frames_count = arg->block_size;
      // process
      plugin->process(plugin,&MClapContext);
      // save audio output
      MOutputAudioFile.write(arg->channels,arg->block_size, MAudioOutputBuffers);

      // prepare for next block
      if (arg->input_midi) {
        deleteInputEvents();
      }

      MCurrentSample += arg->block_size; // samples_per_block
      MCurrentTime += seconds_per_block;
    }

    printf("> finished processing\n");

    // cleanup

    if (arg->input_midi) {
      MMidiFile.unload();
      printf("> unloaded midi file\n");
    }

    MInputAudioFile.close();
    printf("> audio input file closed\n");
    MOutputAudioFile.close();
    printf("> audio output file closed\n");

  }

  //----------

};




#undef MAX_BLOCK_SIZE

//----------------------------------------------------------------------
#endif
