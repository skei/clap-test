
#include <assert.h>
#include <getopt.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "arguments.h"
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

const struct option CMDLINE_ARGS[] = {
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

class TestHost {

//------------------------------
private:
//------------------------------

  arguments_t MArg;
  Host        MHost;
  Entry       MEntry;
  Process     MProcess;

//------------------------------
public:
//------------------------------

  TestHost() {
  }

  //----------

  ~TestHost() {
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
    memset(&MArg,0,sizeof(arguments_t));

    char c;
    char* endptr;

    if (argc==1) {
      printUsage(argv[0]);
      return false;
    }

    else {
      while ((c = getopt_long(argc,argv,OPTION_STRING,CMDLINE_ARGS,NULL)) > 0) {
        switch (c) {

          case 'h':
            printUsage(argv[0]);
            break;

          case 'P':
            MArg.plugin_path = optarg;
            break;

          case 'I':
            MArg.plugin_index = strtof(optarg, &endptr);
            if (endptr == optarg) {
              printf("invalid index: %s\n\n", optarg);
              return false;
            }
            break;

          case 'i':
            MArg.input_audio = optarg;
            if (endptr == optarg) {
              printf("invalid input wav: %s\n\n", optarg);
              return false;
            }
            break;

          case 'o':
            MArg.output_audio = optarg;
            if (endptr == optarg) {
              printf("invalid output wav: %s\n\n", optarg);
              return false;
            }
            break;

          case 'm':
            MArg.input_midi = optarg;
            if (endptr == optarg) {
              printf("invalid input midi: %s\n\n", optarg);
              return false;
            }
            break;

          case 's':
            MArg.sample_rate = strtof(optarg, &endptr);
            if (endptr == optarg) {
              printf("invalid sample rate: %s\n\n", optarg);
              return false;
            }
            break;

          case 'b':
            MArg.block_size = strtol(optarg, &endptr, 0);
            if ((endptr == optarg) || (MArg.block_size <= 0)) {
              printf("invalid block size: %s\n\n", optarg);
              return false;
            }
            break;

          case 'c':
            MArg.channels = strtol(optarg, &endptr, 0);
            if ((endptr == optarg) || (MArg.channels <= 0)) {
              printf("invalid channel count: %s\n\n", optarg);
              return 0;
            }
            break;

          case 'd':
            MArg.decay_seconds = strtof(optarg, &endptr);
            if (endptr == optarg) {
              printf("invalid decay_seconds: %s\n\n", optarg);
              return 0;
            }
            break;

          case 'l':
            MArg.do_list_plugins = 1;
            break;

          case 'D':
            MArg.do_print_descriptor = 1;
            break;

          case 'z':
            MArg.do_fuzz_blocksize = 1;
            break;

          //default:
          //  break;

        } // switch
      } // while
    } //argc
    return true;
  }

  //----------

  Instance* startPlugin(const char* path, uint32_t index,double samplerate) {
    //printf("> creating plugin '%s' (index %i)\n",path,index);
    Instance* instance = MEntry.createInstance(path,index);
    if (!instance) {
      //printf("! couldn't create plugin\n");
      return NULL;
    }
    //printf("> plugin created\n");
    bool result = instance->activate(samplerate);
    if (!result) {
      printf("! couldn't activate plugin\n");
      //destroy
      MEntry.destroyInstance(instance);
      delete instance;
      return NULL;
    }
    printf("> plugin activated\n");

    result = instance->start_processing();
    if (!result) {
      printf("! couldn't start processing\n");
      //deactivate
      instance->deactivate();
      //destroy
      MEntry.destroyInstance(instance);
      delete instance;
      return NULL;
    }
    printf("> started processing\n");
    return instance;
  }

  //----------

  void stopPlugin(Instance* instance) {
    instance->stop_processing();
    printf("+ stopped processing\n");
    instance->deactivate();
    printf("+ deactivated plugin\n");
    MEntry.destroyInstance(instance);
    delete instance;
  }

  //----------

  int main(int argc, char** argv) {
    if (parseArguments(argc,argv)) {
      if (MEntry.load(MArg.plugin_path)) {
        if (MArg.do_list_plugins) {
          MEntry.listPlugins();
        }
        else if (MArg.do_print_descriptor) {
          MEntry.printDescriptor(MArg.plugin_index);
        }
        else {
          Instance* instance = startPlugin(MArg.plugin_path,MArg.plugin_index,MArg.sample_rate);
          if (!instance) {
            printf("!! error starting plugin!\n");
            return -1;
          }
          else {
            instance->printInfo();
            MProcess.process(instance,&MArg);
            stopPlugin(instance);
          }
        }
        MEntry.unload();
      }
      else {
        printf("!! couldn't load plugin\n");
        return -1;
      }
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
  TestHost testhost;
  return testhost.main(argc,argv);
}
