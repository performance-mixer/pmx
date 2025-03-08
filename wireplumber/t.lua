local pmx_filter_builder = {}

local layer_filter_graph_definition = [[
{
  "media.name": "Pmx/Layer",
  "node.description": "PMX Layer Mixer",
  "node.autoconnect": false,
  "filter.graph": {
    "nodes": [
      {
        "name": "Compressor-A",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-A",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Compressor-B",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-B",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Mixer-L",
        "type": "builtin",
        "label": "mixer",
        "control": {}
      },
      {
        "name": "Mixer-R",
        "type": "builtin",
        "label": "mixer",
        "control": {}
      }
    ],
    "links": [
      {
        "output": "Compressor-A:out_l",
        "input": "Equalizer-A:lv2_audio_in_1"
      },
      {
        "output": "Compressor-A:out_r",
        "input": "Equalizer-A:lv2_audio_in_2"
      },
      {
        "output": "Compressor-B:out_l",
        "input": "Equalizer-B:lv2_audio_in_1"
      },
      {
        "output": "Compressor-B:out_r",
        "input": "Equalizer-B:lv2_audio_in_2"
      },
      {
        "output": "Equalizer-A:lv2_audio_out_1",
        "input": "Mixer-L:In 1"
      },
      {
        "output": "Equalizer-A:lv2_audio_out_2",
        "input": "Mixer-R:In 1"
      },
      {
        "output": "Equalizer-B:lv2_audio_out_1",
        "input": "Mixer-L:In 2"
      },
      {
        "output": "Equalizer-B:lv2_audio_out_2",
        "input": "Mixer-R:In 2"
      }
    ],
    "inputs": [
      "Compressor-A:in_l",
      "Compressor-A:in_r",
      "Compressor-B:in_l",
      "Compressor-B:in_r"
    ],
    "outputs": [
      "Mixer-L:Out",
      "Mixer-R:Out"
    ]
  },
  "capture.props": {
    "node.name": "pmx-layer-channels-ins",
    "node.autoconnect": false
  },
  "playback.props": {
    "node.name": "pmx-layer-channels-outs",
    "node.autoconnect": true
  }
}
]]

function pmx_filter_builder.build_layer_mixer(local_module)
	print("Building layer mixer")
	return local_module("libpipewire-module-filter-chain", layer_filter_graph_definition, {})
end

pmx_filter_builder.build_layer_mixer(LocalModule)
