
#include <assert.h>
#include <getopt.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entry.h"
#include "process.h"

#include "audio_file.h"
#include "midi_file.h"
#include "midi_player.h"

//#define PRINT printf
//#define PRINT(...) {}

#define AUDIO_SIZE (16384)

//----------------------------------------------------------------------
//
//
//
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
  int         do_list_plugins;
  int         do_print_descriptor;
  int         do_fuzz_blocksize;
} arguments_t;

//----------------------------------------------------------------------

/*
  name    the name of the long option.
  has_arg no_argument (or 0) if the option does not take an argument;
          required_argument (or 1) if the option requires an argument; or
          optional_argument (or 2) if the option takes an optional argument.
  flag    specifies how results are returned for a long option. If flag is
          NULL, then getopt_long() returns val. (For example, the calling
          program may set val to the equivalent short option character.)
          Otherwise, getopt_long() returns 0, and flag points to a variable
          which is set to val if the option is found, but left unchanged if
          the option is not found.
  val     the value to return, or to load into the variable pointed to by flag
*/

const struct option cmdline_args[] = {
  {"help",              no_argument,       0, 'h'},
  {"plugin",            required_argument, 0, 'p'},
  {"index",             required_argument, 0, 'i'},
  {"input-wav",         required_argument, 0, 'i'},
  {"output-wav",        required_argument, 0, 'o'},
  {"input-midi",        required_argument, 0, 'm'},
  {"sample-rate",       required_argument, 0, 's'},
  {"block-size",        required_argument, 0, 'b'},
  {"channels",          required_argument, 0, 'c'},
  {"decay-seconds",     required_argument, 0, 'd'},
  {"list-plugins",      no_argument,       0, 'l'},
  {"print-descriptor",  no_argument,       0, 'D'},
  {"fuzz-block-size",   no_argument,	     0, 'z'},
//{"automate",          required_argument, 0, 'a'},
//{"profile",           required_argument, 0, 'p'},
//{"fx-store",          required_argument, 0, 'f'},
//{"chunk",             required_argument, 0, 'k'},
  {NULL}
};

//----------

/*
  optstring is a string containing the legitimate option characters. If such a
  character is followed by a colon, the option requires an argument, so
  getopt() places a pointer to the following text in the same argv-element, or
  the text of the following argv-element, in optarg. Two colons mean an option
  takes an optional arg; if there is text in the current argv-element (i.e., in
  the same word as the option name itself, for example, "-oarg"), then it is
  returned in optarg, otherwise optarg is set to zero. This is a GNU extension.
  If optstring contains W followed by a semicolon, then -W foo is treated as
  the long option --foo. (The -W option is reserved by POSIX.2 for
  implementation extensions.)
*/

const char* OPTION_STRING = "hP:I:i:o:m:s:b:c:d:lDz";

const char* USAGE_STRING =
  //"\nusage: %s [options...] <plugin object> [sub-plugin ID]\n"
  "usage: %s [options...]\n"
  "  -h, --help                     display this help\n"
  "  -P, --plugin <path>            plugin\n"
  "  -I, --index <num>              plugin <index>\n"
  "  -i, --input-wav <path>         input wav file\n"
  "  -o, --output-wav <path>        output wav file\n"
  "  -m, --input-midi <path>        input midi file\n"
  "  -s, --sample-rate <samples>    sample rate\n"
  "  -b, --block-size <samples>     block size\n"
  "  -c, --channels <count>         channels\n"
  "  -d, --decay-seconds <seconds>  decay-seconds\n"
  "  -l, --list-plugins             list plugins\n"
  "  -D, --print-descriptor         print descriptor\n"
  "  -z, --fuzz-block-size          fuzz block size\n";

//----------------------------------------------------------------------
//
// clap-test
//
//----------------------------------------------------------------------

class CLAP_Test {

//------------------------------
private:
//------------------------------

  arguments_t MArguments;
  Host        MHost;
  Entry       MEntry;

//------------------------------
public:
//------------------------------

  CLAP_Test() {
  }

  //----------

  ~CLAP_Test() {
  }

//------------------------------
public:
//------------------------------

  void printUsage(const char* APath) {
    const char* filename = get_filename_from_path(APath);
    printf(USAGE_STRING,filename);
  }

  //----------

  bool parseArguments(int argc, char** argv) {
    memset(&MArguments,0,sizeof(arguments_t));

    char c;
    char* endptr;

    if (argc==1) {
      printUsage(argv[0]);
      return false;
    }

    else {
      while ((c = getopt_long(argc,argv,OPTION_STRING,cmdline_args,NULL)) > 0) {
        switch (c) {

          case 'h':
            printUsage(argv[0]);
            break;

          case 'P':
            MArguments.plugin_path = optarg;
            break;

          case 'I':
            MArguments.plugin_index = strtof(optarg, &endptr);
            if (endptr == optarg) {
              printf("invalid index: %s\n\n", optarg);
              return false;
            }
            break;

          case 'i':
            MArguments.input_audio = optarg;
            if (endptr == optarg) {
              printf("invalid input wav: %s\n\n", optarg);
              return false;
            }
            break;

          case 'o':
            MArguments.output_audio = optarg;
            if (endptr == optarg) {
              printf("invalid output wav: %s\n\n", optarg);
              return false;
            }
            break;

          case 'm':
            MArguments.input_midi = optarg;
            if (endptr == optarg) {
              printf("invalid input midi: %s\n\n", optarg);
              return false;
            }
            break;

          case 's':
            MArguments.sample_rate = strtof(optarg, &endptr);
            if (endptr == optarg) {
              printf("invalid sample rate: %s\n\n", optarg);
              return false;
            }
            break;

          case 'b':
            MArguments.block_size = strtol(optarg, &endptr, 0);
            if ((endptr == optarg) || (MArguments.block_size <= 0)) {
              printf("invalid block size: %s\n\n", optarg);
              return false;
            }
            break;

          case 'c':
            MArguments.channels = strtol(optarg, &endptr, 0);
            if ((endptr == optarg) || (MArguments.channels <= 0)) {
              printf("invalid channel count: %s\n\n", optarg);
              return 0;
            }
            break;

          case 'd':
            MArguments.decay_seconds = strtof(optarg, &endptr);
            if (endptr == optarg) {
              printf("invalid decay_seconds: %s\n\n", optarg);
              return 0;
            }
            break;

          case 'l':
            MArguments.do_list_plugins = 1;
            break;

          case 'D':
            MArguments.do_print_descriptor = 1;
            break;

          case 'z':
            MArguments.do_fuzz_blocksize = 1;
            break;

          //default:
          //  break;

        } // switch
      } // while
    } //argc
    return true;
  }

  //----------

  int main(int argc, char** argv) {

    if (parseArguments(argc,argv)) {

      //DEBUG: audio

      if (MArguments.input_audio) {

        AudioFile in_audio;
        AudioFile out_audio;
        float buffer1[AUDIO_SIZE];
        float buffer2[AUDIO_SIZE];
        float* buffers[2] = { buffer1, buffer2 };

        printf("\n");
        printf("loading audio file: %s\n",MArguments.input_audio);
        printf("\n");

        if (in_audio.open(MArguments.input_audio)) {
          SF_INFO* info = in_audio.getInfo();
          printf("  MInfo.frames      %i\n",(int)info->frames);
          printf("  MInfo.samplerate  %i\n",info->samplerate);
          printf("  MInfo.channels    %i\n",info->channels);
          printf("  MInfo.format      %i\n",info->format);
          printf("  MInfo.sections    %i\n",info->sections);
          printf("  MInfo.seekable    %i\n",info->seekable);
          printf("\n");
          printf("reading from file\n");
          in_audio.read(2,AUDIO_SIZE,buffers);
          printf("read ok\n");
          in_audio.close();
        }
        else printf("couldn't open audio input: %s\n",MArguments.input_audio);

        printf("\n");
        printf("saving audio file: %s\n",MArguments.output_audio);
        printf("\n");

        if (out_audio.open(MArguments.output_audio,AUDIO_FILE_WRITE,44100,2)) {
          out_audio.write(2,AUDIO_SIZE,buffers);
          printf("write ok\n");
          out_audio.close();
        }
      }

      //DEBUG: midi

      if (MArguments.input_midi) {

        MidiFile midifile;
        MidiPlayer player;
        /*int result;
        result =*/ midifile.load(MArguments.input_midi);
        MidiSequence* seq = midifile.getMidiSequence();
        seq->calc_time();

        //midifile.print();

        printf("\n");
        printf("playing midi file\n");
        printf("\n");

        player.initialize(&midifile,44100,0);

        //player.process(44100);

        for (uint32_t min_=0; min_<5; min_++) {
          for (uint32_t sec_=0; sec_<60; sec_++) {
            printf("sec %i\n",sec_);
            for (uint32_t spl_=0; spl_<44100; spl_++) {
              player.process(44100);
            }
          }
        }

        player.cleanup();

        printf("finished playing midi file\n");
        midifile.unload();
        printf("unloaded midi file\n");
        printf("\n");
      }

      //

      if (MEntry.load(MArguments.plugin_path)) {

        // list plugins

        if (MArguments.do_list_plugins) {
          MEntry.listPlugins();
        }

        // print descriptor

        else if (MArguments.do_print_descriptor) {
          MEntry.printDescriptor(MArguments.plugin_index);
        }

        else {

        // process

        //----------

          Instance* instance = MEntry.createInstance(MArguments.plugin_path,MArguments.plugin_index);
          if (instance) {
            instance->printInfo();
            if (instance->activate(MArguments.sample_rate)) {
              printf("# plugin activated\n");
              if (instance->start_processing()) {
                printf("# started processing...\n");

                process_audio(instance->getClapPlugin());

                instance->stop_processing();
                printf("# stopped processing\n");
              }
              else {
                printf("! error: couldn't start processing\n");
                return false;
              }
              instance->deactivate();
              printf("# deactivated\n");
            }
            else {
              printf("! error: couldn't activate\n");
              return false;
            }
            MEntry.destroyInstance(instance);
          }
          delete instance;

        //----------

        }
      }
      MEntry.unload();
    }
    return 0;
  }

  //----------

};

//----------------------------------------------------------------------
//
// main
//
//----------------------------------------------------------------------

int main(int argc, char** argv) {
  CLAP_Test test;
  return test.main(argc,argv);
}
