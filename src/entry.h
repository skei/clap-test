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
    printf("Loading binary '%s'\n",APath);
    MLibHandle = open_library(APath);
    if (!MLibHandle) {
      printf("* Error: Couldn't open '%s'\n", APath);
      return false;
    }
    printf("Binary loaded\n");
    printf("Getting 'clap_plugin_entry'\n");
    MClapEntry = (struct clap_plugin_entry*)get_library_symbol(MLibHandle,"clap_plugin_entry");
    if (!MClapEntry) {
      printf("* Error: Couldn't find 'clap_plugin_entry'\n");
      close_library(MLibHandle);
      return 0;
    }
    printf("Found 'clap_plugin_entry'\n");
    get_path_only(MPathOnly,APath);
    MClapEntry->init(MPathOnly);

    return 1;
  }

  //----------

  // entry.deinit
  // unload .so

  void unload() {
    printf("Unloading binary\n");
    MClapEntry->deinit();
    close_library(MLibHandle);
    printf("Binary unloaded\n");
  }

  //----------

  // create and initialize a plugin instance

  Instance* createInstance(const char* APath, uint32_t AIndex) {
    printf("Creating plugin (index %i)\n",AIndex);

    if (AIndex >= MClapEntry->get_plugin_count()) {
      printf("* Error: Index out of bounds\n");
      return NULL;
    }

    const clap_plugin_descriptor* descriptor = MClapEntry->get_plugin_descriptor(AIndex);
    if (!descriptor) {
      printf("* Error: Couldn't get descriptor\n");
      return NULL;
    }

    printf("Creating plugin instance\n");
    const clap_plugin* plugin = MClapEntry->create_plugin( MHost.getClapHost(), descriptor->id );
    if (!plugin) {
      printf("* Error: Couldn't create plugin instance\n");
      return NULL;
    }
    printf("Plugin instance created\n");

    bool result = plugin->init(plugin);
    if (!result) {
      printf("* Error: Couldn't initialize plugin instance\n");
      return NULL;
    }
    printf("Plugin instance initialized\n");

    Instance* instance = new Instance(plugin);
    return instance;

  }

  //----------

  // destroy instance

  void destroyInstance(Instance* AInstance) {
    const clap_plugin* plugin = AInstance->getClapPlugin();
    printf("Destroying plugin instance\n");
    plugin->destroy(plugin);
    printf("Plugin instance destroyed\n");
  }

  //----------

  // list all (sub-) plugins in a binary .so

  void listPlugins() {
    uint32_t plugin_count = MClapEntry->get_plugin_count();
    printf("> found %i plugins\n",plugin_count);
    for (uint32_t i=0; i<plugin_count; i++) {
      const clap_plugin_descriptor* descriptor = MClapEntry->get_plugin_descriptor(i);
      printf("  %i. name '%s' id '%s'\n",i,descriptor->name,descriptor->id);
    }
  }

  //----------

  // print out descriptor of selected (sub-) plugin index

  void printDescriptor(uint32_t AIndex) {
    const clap_plugin_descriptor* descriptor = MClapEntry->get_plugin_descriptor(AIndex);
    if (descriptor) {
      printf("Descriptor:\n");
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
