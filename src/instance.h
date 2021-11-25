#ifndef instance_included
#define instance_included
//----------------------------------------------------------------------

/*
  clap_plugin* 'wrapper'
  PluginInstance
*/

//----------------------------------------------------------------------

#include "clap/all.h"

//----------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------


class Instance {

//------------------------------
private:
//------------------------------

  const clap_plugin*                    MClapPlugin;
  const clap_plugin_audio_ports_config* MClapAudioPortsConfig;
  const clap_plugin_audio_ports*        MClapAudioPorts;
  const clap_plugin_event_filter*       MClapEventFilter;
  const clap_plugin_fd_support*         MClapFdSupport;
  const clap_plugin_gui*                MClapGui;
  const clap_plugin_gui_x11*            MClapGuiX11;
  const clap_plugin_latency*            MClapLatency;
  const clap_plugin_note_name*          MClapNoteName;
  const clap_plugin_params*             MClapParams;
  const clap_plugin_render*             MClapRender;
  const clap_plugin_state*              MClapState;
  const clap_plugin_vst2_convert*       MClapVst2Convert;
  const clap_plugin_vst3_convert*       MClapVst3Convert;

//------------------------------
public:
//------------------------------

  Instance(const clap_plugin* AClapPlugin) {
    MClapPlugin = AClapPlugin;
    init_instance(AClapPlugin);
  }

  //----------

  ~Instance() {
  }

//------------------------------
public:
//------------------------------

  const clap_plugin* getClapPlugin() {
    return MClapPlugin;
  }

//------------------------------
public:
//------------------------------

  // ask plugin (instance) about extensions..

  void init_instance(const clap_plugin* plugin) {
    MClapAudioPortsConfig = (const clap_plugin_audio_ports_config*)plugin->get_extension(plugin,CLAP_EXT_AUDIO_PORTS_CONFIG);
    MClapAudioPorts       = (const clap_plugin_audio_ports*)plugin->get_extension(plugin,CLAP_EXT_AUDIO_PORTS);
    MClapEventFilter      = (const clap_plugin_event_filter*)plugin->get_extension(plugin,CLAP_EXT_EVENT_FILTER);
    MClapFdSupport        = (const clap_plugin_fd_support*)plugin->get_extension(plugin,CLAP_EXT_FD_SUPPORT);
    MClapGui              = (const clap_plugin_gui*)plugin->get_extension(plugin,CLAP_EXT_GUI);
    MClapGuiX11           = (const clap_plugin_gui_x11*)plugin->get_extension(plugin,CLAP_EXT_GUI_X11);
    MClapLatency          = (const clap_plugin_latency*)plugin->get_extension(plugin,CLAP_EXT_LATENCY);
    MClapNoteName         = (const clap_plugin_note_name*)plugin->get_extension(plugin,CLAP_EXT_NOTE_NAME);
    MClapParams           = (const clap_plugin_params*)plugin->get_extension(plugin,CLAP_EXT_PARAMS);
    MClapRender           = (const clap_plugin_render*)plugin->get_extension(plugin,CLAP_EXT_RENDER);
    MClapState            = (const clap_plugin_state*)plugin->get_extension(plugin,CLAP_EXT_STATE);
    MClapVst2Convert      = (const clap_plugin_vst2_convert*)plugin->get_extension(plugin,CLAP_EXT_VST2_CONVERT);
    MClapVst3Convert      = (const clap_plugin_vst3_convert*)plugin->get_extension(plugin,CLAP_EXT_VST3_CONVERT);
  }

  //----------

  bool activate(float ASampleRate) {
    return MClapPlugin->activate(MClapPlugin,ASampleRate);
  }

  //----------

  void deactivate() {
    MClapPlugin->deactivate(MClapPlugin);
  }

  //----------

  bool start_processing() {
    return MClapPlugin->start_processing(MClapPlugin);
  }

  //----------

  void stop_processing() {
    MClapPlugin->stop_processing(MClapPlugin);
  }

  //----------

  // print port info (see printInfo)

  void printPortInfo(clap_audio_port_info* info) {
    printf("    name          %s\n",info->name);
    printf("    channel count %i\n",info->channel_count);
    printf("    channel map   %i\n",info->channel_map);
    printf("    sample-size   %i\n",info->sample_size);
    printf("    is-main       %s\n",info->is_main ? "true" : "false");
    printf("    in-place      %s\n",info->in_place ? "true" : "false");
    printf("    is-cv         %s\n",info->is_cv ? "true" : "false");
  }

  //----------

  // print some info

  void printInfo() {
    int i,num;
    clap_audio_port_info info;

    printf("# audio-ports (%s)\n", (MClapAudioPorts) ? "yes" : "no"  );
    if (MClapAudioPorts) {
      num = MClapAudioPorts->count(MClapPlugin,true);
      printf("  %i audio input ports\n",num);
      for (i=0; i<num; i++) {
        MClapAudioPorts->get(MClapPlugin,i,true,&info);
        printPortInfo(&info);
      }
      num = MClapAudioPorts->count(MClapPlugin,false);
      printf("  %i audio output ports\n",num);
      for (i=0; i<num; i++) {
        MClapAudioPorts->get(MClapPlugin,i,false,&info);
        printPortInfo(&info);
      }
    }

    printf("# params (%s)\n", (MClapParams) ? "yes" : "no"  );
    if (MClapParams) {
      num = MClapParams->count(MClapPlugin);
      printf("  %i params\n",num);
    }
  }

  //----------

};

//----------------------------------------------------------------------
#endif

