#ifndef instance_included
#define instance_included
//----------------------------------------------------------------------

/*
  clap_plugin* 'wrapper'
  HostedPluginInstance
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

  const clap_plugin_audio_ports_config* MClapExtAudioPortsConfig;
  const clap_plugin_audio_ports*        MClapExtAudioPorts;
  const clap_plugin_event_filter*       MClapExtEventFilter;
  const clap_plugin_fd_support*         MClapExtFdSupport;
  const clap_plugin_gui*                MClapExtGui;
  const clap_plugin_gui_x11*            MClapExtGuiX11;
  const clap_plugin_latency*            MClapExtLatency;
  const clap_plugin_note_name*          MClapExtNoteName;
  const clap_plugin_params*             MClapExtParams;
  const clap_plugin_render*             MClapExtRender;
  const clap_plugin_state*              MClapExtState;
  const clap_plugin_vst2_convert*       MClapExtVst2Convert;
  const clap_plugin_vst3_convert*       MClapExtVst3Convert;

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
  // TODO: check more extensions

  void init_instance(const clap_plugin* plugin) {
    MClapExtAudioPortsConfig = (const clap_plugin_audio_ports_config*)plugin->get_extension(plugin,CLAP_EXT_AUDIO_PORTS_CONFIG);
    MClapExtAudioPorts       = (const clap_plugin_audio_ports*)plugin->get_extension(plugin,CLAP_EXT_AUDIO_PORTS);
    MClapExtEventFilter      = (const clap_plugin_event_filter*)plugin->get_extension(plugin,CLAP_EXT_EVENT_FILTER);
    MClapExtFdSupport        = (const clap_plugin_fd_support*)plugin->get_extension(plugin,CLAP_EXT_FD_SUPPORT);
    MClapExtGui              = (const clap_plugin_gui*)plugin->get_extension(plugin,CLAP_EXT_GUI);
    MClapExtGuiX11           = (const clap_plugin_gui_x11*)plugin->get_extension(plugin,CLAP_EXT_GUI_X11);
    MClapExtLatency          = (const clap_plugin_latency*)plugin->get_extension(plugin,CLAP_EXT_LATENCY);
    MClapExtNoteName         = (const clap_plugin_note_name*)plugin->get_extension(plugin,CLAP_EXT_NOTE_NAME);
    MClapExtParams           = (const clap_plugin_params*)plugin->get_extension(plugin,CLAP_EXT_PARAMS);
    MClapExtRender           = (const clap_plugin_render*)plugin->get_extension(plugin,CLAP_EXT_RENDER);
    MClapExtState            = (const clap_plugin_state*)plugin->get_extension(plugin,CLAP_EXT_STATE);
    MClapExtVst2Convert      = (const clap_plugin_vst2_convert*)plugin->get_extension(plugin,CLAP_EXT_VST2_CONVERT);
    MClapExtVst3Convert      = (const clap_plugin_vst3_convert*)plugin->get_extension(plugin,CLAP_EXT_VST3_CONVERT);
  }

  //----------

  bool activate(float ASampleRate, uint32_t AMinFrames, uint32_t AMaxFrames) {
    // 0.14.0
    return MClapPlugin->activate(MClapPlugin,ASampleRate,AMinFrames,AMaxFrames);
    // 0.15.0
    //return MClapPlugin->activate(MClapPlugin,ASampleRate,1,4096);
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
    printf("    id            %i\n",info->id);
    printf("    channel count %i\n",info->channel_count);
    printf("    channel map   %i\n",info->channel_map);
    printf("    sample-size   %i\n",info->sample_size);
    printf("    is-main       %s\n",info->is_main ? "true" : "false");
    printf("    in-place      %s\n",info->in_place ? "true" : "false");
    printf("    is-cv         %s\n",info->is_cv ? "true" : "false");
  }

  //----------

  // print some info
  // TODO

  void printInfo() {
    int i,num;
    clap_audio_port_info info;

    printf("# audio-ports (%s)\n", (MClapExtAudioPorts) ? "yes" : "no"  );
    if (MClapExtAudioPorts) {
      num = MClapExtAudioPorts->count(MClapPlugin,true);
      printf("  %i audio input ports\n",num);
      for (i=0; i<num; i++) {
        MClapExtAudioPorts->get(MClapPlugin,i,true,&info);
        printPortInfo(&info);
      }
      num = MClapExtAudioPorts->count(MClapPlugin,false);
      printf("  %i audio output ports\n",num);
      for (i=0; i<num; i++) {
        MClapExtAudioPorts->get(MClapPlugin,i,false,&info);
        printPortInfo(&info);
      }
    }

    printf("# params (%s)\n", (MClapExtParams) ? "yes" : "no"  );
    if (MClapExtParams) {
      num = MClapExtParams->count(MClapPlugin);
      printf("  %i params\n",num);
    }
  }

  //----------

};

//----------------------------------------------------------------------
#endif

