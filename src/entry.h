#ifndef entry_included
#define entry_included
//----------------------------------------------------------------------

/*
  clap_plugin_entry*
  HostedPluginEntry
*/

//----------------------------------------------------------------------

#include "clap/all.h"
#include "system.h"
#include "utils.h"
#include "host.h"
#include "instance.h"

//----------------------------------------------------------------------

class Entry {

//------------------------------
private:
//------------------------------

  void*                     MLibHandle;
  const char*               MPluginPath;
  char                      MPathOnly[512];
  struct clap_plugin_entry* MClapEntry;
  Host                      MHost;

//------------------------------
public:
//------------------------------

  Entry() {
  }

  //----------

  ~Entry() {
  }

//------------------------------
public:
//------------------------------

  // load .so
  // entry.init

  bool load(const char* APath) {
    MPluginPath = APath;
    printf("# Loading: %s\n",APath);
    MLibHandle = open_library(APath);
    if (!MLibHandle) {
      printf("! Error: Couldn't open %s\n", APath);
      return false;
    }
    MClapEntry = (struct clap_plugin_entry*)get_library_symbol(MLibHandle,"clap_plugin_entry");
    if (!MClapEntry) {
      printf("! Error: Couldn't find 'clap_plugin_entry'\n");
      close_library(MLibHandle);
      return 0;
    }
    get_path_only(MPathOnly,APath);
    MClapEntry->init(MPathOnly);
    return 1;
  }

  //----------

  // entry.deinit
  // unload .so

  void unload() {
    printf("# unloading\n");
    MClapEntry->deinit();
    close_library(MLibHandle);
  }

  //----------

  // create and initialize a plugin instance

  Instance* createInstance(const char* APath, uint32_t AIndex) {
    printf("# creating plugin instance, index %i\n",AIndex);
    if (AIndex < MClapEntry->get_plugin_count()) {
      const clap_plugin_descriptor* descriptor = MClapEntry->get_plugin_descriptor(AIndex);
      if (descriptor) {
        printf("  (host: %p, plugin_id: %s\n",MHost.getClapHost(),descriptor->id);
        const clap_plugin* plugin = MClapEntry->create_plugin( MHost.getClapHost(), descriptor->id );
        if (plugin) {
          printf("# plugin created\n");
          if (plugin->init(plugin)) {
            printf("# plugin initialized\n");
            Instance* instance = new Instance(plugin);
            return instance;
          }
          else {
            printf("! error: plugin initialization failed\n");
            return NULL;
          }
        }
        else {
          printf("! error: plugin creation failed\n");
          return NULL;
        }
      }
      else {
        printf("! error: couldn't get descriptor\n");
        return NULL;
      }
    }
    else {
      printf("! error: index out of range\n");
      return NULL;
    }

  }

  //----------

  // destroy instance

  void destroyInstance(Instance* AInstance) {
    const clap_plugin* plugin = AInstance->getClapPlugin();
    plugin->destroy(plugin);
    printf("# plugin destroyed\n");
  }

  //----------

  // list all (sub-) plugins in a binary .so

  void listPlugins() {
    uint32_t plugin_count = MClapEntry->get_plugin_count();
    printf("# found %i plugins:\n",plugin_count);
    for (uint32_t i=0; i<plugin_count; i++) {
      const clap_plugin_descriptor* descriptor = MClapEntry->get_plugin_descriptor(i);
      printf("  %i %s (%s)\n",i,descriptor->name,descriptor->id);
    }
  }

  //----------

  // print out descriptor of selected (sub-) plugin index

  void printDescriptor(uint32_t AIndex) {
    const clap_plugin_descriptor* descriptor = MClapEntry->get_plugin_descriptor(AIndex);
    if (descriptor) {
      printf("  clap_version: %i.%i.%i\n",descriptor->clap_version.major,descriptor->clap_version.minor,descriptor->clap_version.revision);
      printf("  id            %s\n",      descriptor->id);
      printf("  name          %s\n",      descriptor->name);
      printf("  vendor        %s\n",      descriptor->vendor);
      printf("  url           %s\n",      descriptor->url);
      printf("  manual_url    %s\n",      descriptor->manual_url);
      printf("  support_url   %s\n",      descriptor->support_url);
      printf("  version       %s\n",      descriptor->version);
      printf("  description   %s\n",      descriptor->description);
      printf("  plugin_type   %i\n", (int)descriptor->plugin_type);
    }
  }

  //----------

};

//----------------------------------------------------------------------
#endif
