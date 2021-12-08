#ifndef arguments_included
#define arguments_included
//----------------------------------------------------------------------

typedef struct arguments_t {
  const char* plugin_path;
  int         plugin_index;
  const char* input_audio;
  const char* output_audio;
  const char* input_midi;
  const char* output_midi;
  float       sample_rate;
  int         block_size;
  int         channels;
  float       decay_seconds;
  int         animated_parameter;
  int         do_list_plugins;
  int         do_print_descriptor;
  int         do_fuzz_blocksize;
  int         do_animate;
} arguments_t;

//----------------------------------------------------------------------
#endif
