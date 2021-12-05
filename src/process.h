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


  MidiFile              MMidiFile;
  MidiPlayer            MMidiPlayer;
  MidiSequence*         MMidiSequence;

  MidiEvents            MMidiInputEvents;
  clap_events           MClapInputEvents;

  AudioFile             MInputAudio;
  AudioFile             MOutputAudio;
  float                 MAudioInputBuffer1[MAX_BLOCK_SIZE];
  float                 MAudioInputBuffer2[MAX_BLOCK_SIZE];
  float*                MAudioInputBuffers[2] = { MAudioInputBuffer1, MAudioInputBuffer2 };
  float                 MAudioOutputBuffer1[MAX_BLOCK_SIZE];
  float                 MAudioOutputBuffer2[MAX_BLOCK_SIZE];
  float*                MAudioOutputBuffers[2] = { MAudioOutputBuffer1, MAudioOutputBuffer2 };

  uint32_t              MCurrentSample  = 0;
  float                 MCurrentTime    = 0.0;
  Instance*             MInstance       = NULL;
  float                 MSampleRate     = 0.0;


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

  void clearInputEvents() {
    MMidiInputEvents.clear();
    MClapInputEvents.clear();
  }

  //----------

  uint32_t getNumInputEvents() {
    return MMidiInputEvents.size();
  }

  //----------

  const clap_event* getInputEvent(uint32_t index) {
    MidiEvent* midievent = MMidiInputEvents[index];
    float block_time = midievent->time - MCurrentTime;
    int32_t offset = floorf(block_time * MSampleRate);
    uint8_t msg1 = MMidiInputEvents[index]->msg1;
    uint8_t msg2 = MMidiInputEvents[index]->msg2;
    uint8_t msg3 = MMidiInputEvents[index]->msg3;
    printf("  process.inputevent offset %i : %02x %02x %02x\n",offset,msg1,msg2,msg3);

    clap_event* event = (clap_event*)malloc(sizeof(clap_event));
    MClapInputEvents.push_back(event);
    memset(event,0,sizeof(clap_event));
    event->type = CLAP_EVENT_MIDI;
    event->time = offset;
    event->midi.data[0] = 1;//msg1; ..... these doesn't survive..
    event->midi.data[1] = 2;//msg2;
    event->midi.data[2] = 3;//msg3;

    return event;
  }

  //----------

  void deleteInputEvents() {
    for (uint32_t i=0; i<MClapInputEvents.size(); i++) {
      free( MClapInputEvents[i] );
    }
    MClapInputEvents.clear();
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

//  void initBlockEvents() {
//    MBlockEvents.clear();
//  }

  //----------




  //----------

  void process(Instance* instance, arguments_t* arg) {

    MInstance = instance;
    MSampleRate = arg->sample_rate;
    const clap_plugin* plugin = instance->getClapPlugin();
    uint32_t num_samples = 0;

    if (arg->input_midi) {
      /*int result =*/ MMidiFile.load(arg->input_midi);
      //MidiSequence* seq = MMidiFile.getMidiSequence();
      //seq->calc_time();
      MMidiPlayer.initialize(&MMidiFile,44100,0);
      MMidiSequence = MMidiPlayer.getMidiSequence();
      float midi_length = MMidiSequence->length;
      num_samples = arg->sample_rate * midi_length;
    }

    if (arg->input_audio) {
      printf("> opening audio input file: %s\n",arg->input_audio);
      bool result = MInputAudio.open(arg->input_audio,AUDIO_FILE_READ);
      if (!result) {
        printf("! couldn't open audio input file\n");
        return;
      }
      printf("> audio input file opened\n");
      printf("    - length: %i\n",(int)MInputAudio.getInfo()->frames);
      num_samples = MInputAudio.getInfo()->frames;
    }

    if (num_samples == 0) {
      printf("!! num_samples == 0\n");
      return;
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

    if (num_samples >= (arg->sample_rate * 60.0)) {
      printf("! 1 minute.. truncation\n" );
      num_samples = 60.0 * arg->sample_rate;
    }

    uint32_t num_blocks = num_samples / arg->block_size;
    num_blocks += 1; // just to be sure :-)
    float seconds_per_block = arg->block_size / arg->sample_rate;

    MCurrentSample  = 0;
    MCurrentTime    = 0.0;

    //-----

    printf("> processing %i blocks\n",num_blocks);

    for (uint32_t i=0; i<num_blocks; i++) {

      printf("block %i\n",i);

      if (arg->input_midi) {
        clearInputEvents();
        MMidiPlayer.GetEventsForBlock(MCurrentTime,seconds_per_block,&MMidiInputEvents);
        deleteInputEvents();
      }
      else {
        MInputAudio.read(arg->channels,arg->block_size,MAudioInputBuffers);
      }

      MClapContext.steady_time = MCurrentSample;
      MClapContext.frames_count = arg->block_size;

      plugin->process(plugin,&MClapContext);
      MOutputAudio.write(arg->channels,arg->block_size, MAudioOutputBuffers);

      MCurrentSample += arg->block_size; // samples_per_block
      MCurrentTime += seconds_per_block;
    }

    printf("> finished processing\n");

    //-----

    if (arg->input_midi) {
      MMidiFile.unload();
      printf("> unloaded midi file\n");
    }

    MInputAudio.close();
    printf("> audio input file closed\n");
    MOutputAudio.close();
    printf("> audio output file closed\n");
  }

  //----------

};




#undef MAX_BLOCK_SIZE

//----------------------------------------------------------------------
#endif
