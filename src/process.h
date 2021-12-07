#ifndef process_included
#define process_included
//----------------------------------------------------------------------

// in progress!!

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

  AudioFile             MAudioInputFile;
  AudioFile             MAudioOutputFile;
  MidiFile              MMidiFile;
  MidiPlayer            MMidiPlayer;

  Instance*             MInstance       = NULL;
  float                 MSampleRate     = 0.0;
  uint32_t              MCurrentSample  = 0;
  float                 MCurrentTime    = 0.0;
  MidiSequence*         MMidiSequence   = NULL;

  MidiEvents            MMidiInputEvents;
  clap_events           MClapInputEvents;

  alignas(32) float MAudioInputBuffer1[MAX_BLOCK_SIZE];
  alignas(32) float MAudioInputBuffer2[MAX_BLOCK_SIZE];
  alignas(32) float MAudioOutputBuffer1[MAX_BLOCK_SIZE];
  alignas(32) float MAudioOutputBuffer2[MAX_BLOCK_SIZE];

  //alignas(32)
  float* MAudioInputBuffers[2] = {
    MAudioInputBuffer1,
    MAudioInputBuffer2
  };

  //alignas(32)
  float* MAudioOutputBuffers[2] = {
    MAudioOutputBuffer1,
    MAudioOutputBuffer2
  };

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
    return MClapInputEvents.size();
  }

  //----------

  const clap_event* input_events_get(uint32_t index) {
    return MClapInputEvents[index];
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
  uint32_t process_input_events_size(const struct clap_event_list* list) {
    Process* process = (Process*)list->ctx;
    return process->input_events_size();
  }

  //----------

  static
  const clap_event* process_input_events_get(const struct clap_event_list* list, uint32_t index) {
    Process* process = (Process*)list->ctx;
    return process->input_events_get(index);
  }

  //----------

  static
  void process_input_events_push_back(const struct clap_event_list* list, const clap_event *event) {
    Process* process = (Process*)list->ctx;
    process->input_events_push_back(event);
  }

  //----------

  static
  uint32_t process_output_events_size(const struct clap_event_list* list) {
    Process* process = (Process*)list->ctx;
    return process->output_events_size();
  }

  //----------

  static
  const clap_event* process_output_events_get(const struct clap_event_list* list, uint32_t index) {
    Process* process = (Process*)list->ctx;
    return process->output_events_get(index);
  }

  //----------

  static
  void process_output_events_push_back(const struct clap_event_list* list, const clap_event *event) {
    Process* process = (Process*)list->ctx;
    process->output_events_push_back(event);
  }

//------------------------------
public:
//------------------------------

 //TODO: input + output channel counts?

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
    MClapEventInputs.ctx              = this; // NULL;                    // reserved pointer for the list
    MClapEventInputs.size             = &process_input_events_size;
    MClapEventInputs.get              = &process_input_events_get;        // Don't free the return event, it belongs to the list
    MClapEventInputs.push_back        = &process_input_events_push_back;  // Makes a copy of the event
  }

  void prepare_event_outputs() {
    MClapEventOutputs.ctx             = this; // NULL;
    MClapEventOutputs.size            = &process_output_events_size;
    MClapEventOutputs.get             = &process_output_events_get;
    MClapEventOutputs.push_back       = &process_output_events_push_back;
  }

  /*
    CLAP_TRANSPORT_HAS_TEMPO
    CLAP_TRANSPORT_HAS_BEATS_TIMELINE
    CLAP_TRANSPORT_HAS_SECONDS_TIMELINE
    CLAP_TRANSPORT_HAS_TIME_SIGNATURE
    CLAP_TRANSPORT_IS_PLAYING
    CLAP_TRANSPORT_IS_RECORDING
    CLAP_TRANSPORT_IS_LOOP_ACTIVE
    CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL

    TODO:
      get tempo/timesig from midifile
      increase counter as we render the blocks
  */

  void prepare_transport() {
    MClapTransport.flags              = CLAP_TRANSPORT_IS_PLAYING;
    MClapTransport.song_pos_beats     = 0;
    MClapTransport.song_pos_seconds   = 0;
    MClapTransport.tempo              = 120.0;
    MClapTransport.tempo_inc          = 0.0;
    MClapTransport.bar_start          = 0;
    MClapTransport.bar_number         = 0;
    MClapTransport.loop_start_beats   = 0;
    MClapTransport.loop_end_beats     = 0;
    MClapTransport.loop_start_seconds = 0;
    MClapTransport.loop_end_seconds   = 0;
    MClapTransport.tsig_num           = 4;
    MClapTransport.tsig_denom         = 4;
  }

  void prepare_context(uint32_t channels, uint32_t blocksize, uint32_t latency=0) {
    MClapContext.steady_time          = MCurrentSample;     // a steady sample time counter, requiered
    MClapContext.frames_count         = blocksize;          // number of frame to process
    MClapContext.transport            = &MClapTransport;
    MClapContext.audio_inputs         = &MClapAudioInputs;
    MClapContext.audio_outputs        = &MClapAudioOutputs;

    MClapContext.audio_inputs_count   = 0; // TODO
    MClapContext.audio_outputs_count  = channels;

    MClapContext.in_events            = &MClapEventInputs;
    MClapContext.out_events           = &MClapEventOutputs;
  }

//------------------------------
private:
//------------------------------

  /*
    free all clap_events we (potentially) appended in convertInputEvents
    and reset both MClapInputEvents and MMidiInputEvents
  */

  void clearInputEvents() {
    uint32_t num_events = MClapInputEvents.size();
    for (uint32_t i=0; i<num_events; i++) {
      if (MClapInputEvents[i]) free( MClapInputEvents[i] );
      MClapInputEvents[i] = NULL;
    }
    MClapInputEvents.clear();

    /*
      we don't allocate any midievents
      (just get pointers to existing ones),
      so nothing to free
    */

    MMidiInputEvents.clear();
  }

  //----------

  /*
    called before processing current audiobuffer
    allocates a clap_event for each midi event in MidiInputEvents
    initializes it, and appends it to MClapInputEvents

  */

  void convertInputEvents() {
    uint32_t num_events = MMidiInputEvents.size();
    for (uint32_t i=0; i<num_events; i++) {
      MidiEvent* midievent = MMidiInputEvents[i];
      float   time    = midievent->time - MCurrentTime;
      uint8_t msg1    = midievent->msg1;
      uint8_t msg2    = midievent->msg2;
      uint8_t msg3    = midievent->msg3;
      int32_t offset  = floorf(time * MSampleRate);
      printf("> MIDI : offset %i : %02x %02x %02x\n",offset,msg1,msg2,msg3);
      clap_event* event;
      switch( msg1 & 0xF0) {

        case 0x80: // note off
          event = (clap_event*)malloc(sizeof(clap_event));  // deleted in deleteInputEvents()
          memset(event,0,sizeof(clap_event));
          event->time             = offset;
          event->type             = CLAP_EVENT_NOTE_OFF;
          event->note.port_index  = 0;
          event->note.key         = msg2;
          event->note.channel     = msg1 & 0x0f;
          event->note.velocity    = msg3 / 127.0;
          MClapInputEvents.push_back(event);
          break;

        case 0x90: // note on
          event = (clap_event*)malloc(sizeof(clap_event));  // deleted in deleteInputEvents()
          memset(event,0,sizeof(clap_event));
          event->time             = offset;
          event->type             = CLAP_EVENT_NOTE_ON;
          event->note.port_index  = 0;
          event->note.key         = msg2;
          event->note.channel     = msg1 & 0x0f;
          event->note.velocity    = msg3 / 127.0;
          MClapInputEvents.push_back(event);
          break;

        //case 0xA0: // poly aftertouch
        //  event->type                           = CLAP_EVENT_NOTE_EXPRESSION;
        //  event->note_expression.expression_id  = CLAP_NOTE_EXPRESSION_PRESSURE;
        //  event->note_expression.port_index     = 0;
        //  event->note_expression.key            = msg2;
        //  event->note_expression.channel        = msg1 & 0x0f;
        //  event->note_expression.value          = msg3 / 127.0; // TODO
        //  break;

        //case 0xB0: // control change
        //  break;

        //case 0xC0: // program change
        //  break;

        //case 0xD0: // channel aftertouch
        //  event->type                           = CLAP_EVENT_NOTE_EXPRESSION;
        //  event->note_expression.expression_id  = CLAP_NOTE_EXPRESSION_PRESSURE;
        //  event->note_expression.port_index     = 0;
        //  event->note_expression.key            = msg2;
        //  event->note_expression.channel        = msg1 & 0x0f;
        //  event->note_expression.value          = msg3 / 127.0; // TODO
        //  break;

        //case 0xE0: // pitch bend
        //  event->type                           = CLAP_EVENT_NOTE_EXPRESSION;
        //  event->note_expression.expression_id  = CLAP_NOTE_EXPRESSION_TUNING;
        //  event->note_expression.port_index     = 0;
        //  event->note_expression.key            = msg2;
        //  event->note_expression.channel        = msg1 & 0x0f;
        //  event->note_expression.value          = msg3 / 127.0; // TODO
        //  break;

        //default:
        //  event->type = CLAP_EVENT_MIDI;
        //  event->midi.data[0] = msg1;
        //  event->midi.data[1] = msg2;
        //  event->midi.data[2] = msg3;
        //  break;

      }  // switch
    } // for all events
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
      MMidiPlayer.initialize(&MMidiFile,arg->sample_rate);
      MMidiSequence = MMidiPlayer.getMidiSequence();
      float midi_length = MMidiSequence->length;
      num_samples = arg->sample_rate * midi_length;
    }

    // audio input

    if (arg->input_audio) {
      printf("Opening audio input file: '%s'\n",arg->input_audio);
      bool result = MAudioInputFile.open(arg->input_audio,AUDIO_FILE_READ);
      if (!result) {
        printf("* Error: Couldn't open audio input file\n");
        return;
      }
      printf("Audio input file opened (length = %i frames\n",(int)MAudioInputFile.getInfo()->frames);
      num_samples = MAudioInputFile.getInfo()->frames;
    }

    // audio output

    if (arg->output_audio) {
      printf("Opening audio output file: '%s'\n",arg->output_audio);
      bool result = MAudioOutputFile.open(arg->output_audio,AUDIO_FILE_WRITE,arg->sample_rate,arg->channels);
      if (!result ){
        printf("* Error: Couldn't open audio output file\n");
        return;
      }
      printf("Audio output file opened\n");
    }
    else {
      printf("* Error: No audio output file specified\n");
      return;
    }

    //-----

    printf("> length = %f seconds\n",num_samples / arg->sample_rate);

    if (num_samples == 0) {
      printf("* Error: num_samples == 0\n");
      //return;
    }

    if (num_samples >= (arg->sample_rate * 180.0)) {
      printf("> Truncating to 3 minute\n" );
      num_samples = 180.0 * arg->sample_rate;
    }

    uint32_t num_blocks = num_samples / arg->block_size;
    num_blocks += 1; // just to be sure :-)
    float seconds_per_block = arg->block_size / arg->sample_rate;

    // prepare

    prepare_transport();
    prepare_audio_inputs(arg->channels,0);
    prepare_audio_outputs(arg->channels,0);
    prepare_event_inputs();
    prepare_event_outputs();
    prepare_context(arg->channels,arg->block_size,0);

    MCurrentSample  = 0;    // current position (in samples)
    MCurrentTime    = 0.0;  // current position (in seconds)

    // process all blocks

    printf("Processing %i blocks\n",num_blocks);
    for (uint32_t i=0; i<num_blocks; i++) {
      printf("  block %i (samplepos %i)\n",i,MCurrentSample);

      // events for current block
      if (arg->input_midi) {

        clearInputEvents();
        MMidiPlayer.GetEventsForBlock(MCurrentTime,seconds_per_block,&MMidiInputEvents);
        convertInputEvents();

      }
      //else {
      if (arg->input_audio) {
        MAudioInputFile.read(arg->channels,arg->block_size,MAudioInputBuffers);
      }

      // update current time
      MClapContext.steady_time = MCurrentSample;
      MClapContext.frames_count = arg->block_size;

      // process
      plugin->process(plugin,&MClapContext);

      // save audio output
      MAudioOutputFile.write(arg->channels,arg->block_size, MAudioOutputBuffers);

      MCurrentSample += arg->block_size; // samples_per_block
      MCurrentTime += seconds_per_block;
    }

    printf("Finished processing\n");

    // cleanup

    clearInputEvents();

    if (arg->input_midi) {
      MMidiFile.unload();
      printf("Unloaded midi file\n");
    }

    if (arg->input_audio) {
      MAudioInputFile.close();
      printf("Audio input file closed\n");
    }

    if (arg->output_audio) {
      MAudioOutputFile.close();
      printf("Audio output file closed\n");
    }

  }

  //----------

};




#undef MAX_BLOCK_SIZE

//----------------------------------------------------------------------
#endif
