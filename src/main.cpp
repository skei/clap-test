
#include <assert.h>
#include <getopt.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "mid.h"
#include "wav.h"


#include "clap_entry.hpp"

#include "process.h"

//#define PRINT printf
//#define PRINT(...) {}

//----------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------

typedef struct arguments_t {
  const char* plugin_path;
  int         plugin_index;
  const char* input_wav;
  const char* output_wav;
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

const char* OPTION_STRING = "hP:I:i:o:m:s:b:c:d:lDz";

//----------------------------------------------------------------------
//
// clap-test
//
//----------------------------------------------------------------------

class CLAP_Test {

//------------------------------
private:
//------------------------------

  arguments_t   MArguments;
  CLAP_Host     MHost;
  CLAP_Entry    MEntry;

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
    printf(
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
      "  -z, --fuzz-block-size          fuzz block size\n"
      , filename
    );
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
            MArguments.input_wav = optarg;
            if ((endptr == optarg) || (MArguments.channels <= 0)) {
              printf("invalid input wav: %s\n\n", optarg);
              return false;
            }
            break;

          case 'o':
            MArguments.output_wav = optarg;
            if ((endptr == optarg) || (MArguments.channels <= 0)) {
              printf("invalid output wav: %s\n\n", optarg);
              return false;
            }
            break;

          case 'm':
            MArguments.input_midi = optarg;
            if ((endptr == optarg) || (MArguments.channels <= 0)) {
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
      if (MEntry.load(MArguments.plugin_path)) {
        if (MArguments.do_list_plugins) {
          MEntry.listPlugins();
        }
        else if (MArguments.do_print_descriptor) {
          MEntry.printDescriptor(MArguments.plugin_index);
        }
        else {

          //----------

          CLAP_Instance* instance = MEntry.createInstance(MArguments.plugin_path,MArguments.plugin_index);
          if (instance) {
            instance->printInfo();
            if (instance->activate(MArguments.sample_rate)) {
              printf("# plugin activated\n");
              if (instance->start_processing()) {
                printf("# started processing...\n");

                //process_audio(instance->getClapPlugin());

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
  //PRINT("helo world\n");
  CLAP_Test test;
  return test.main(argc,argv);
}
