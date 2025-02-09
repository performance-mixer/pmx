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

local group_channel_filter_graph_definition = [[
{
  "media.name": "Pmx/Group/{layer_id}",
  "node.description": "PMX Group Channels {layer_id}",
  "filter.graph": {
    "nodes": [
      {
        "name": "Compressor-G1",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-G1",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Compressor-G2",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-G2",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Compressor-G3",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-G3",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Compressor-G4",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-G4",
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
        "output": "Compressor-G1:out_l",
        "input": "Equalizer-G1:lv2_audio_in_1"
      },
      {
        "output": "Compressor-G1:out_r",
        "input": "Equalizer-G1:lv2_audio_in_2"
      },
      {
        "output": "Compressor-G2:out_l",
        "input": "Equalizer-G2:lv2_audio_in_1"
      },
      {
        "output": "Compressor-G2:out_r",
        "input": "Equalizer-G2:lv2_audio_in_2"
      },
      {
        "output": "Compressor-G3:out_l",
        "input": "Equalizer-G3:lv2_audio_in_1"
      },
      {
        "output": "Compressor-G3:out_r",
        "input": "Equalizer-G3:lv2_audio_in_2"
      },
      {
        "output": "Compressor-G4:out_l",
        "input": "Equalizer-G4:lv2_audio_in_1"
      },
      {
        "output": "Compressor-G4:out_r",
        "input": "Equalizer-G4:lv2_audio_in_2"
      },
      {
        "output": "Equalizer-G1:lv2_audio_out_1",
        "input": "Mixer-L:In 1"
      },
      {
        "output": "Equalizer-G1:lv2_audio_out_2",
        "input": "Mixer-R:In 1"
      },
      {
        "output": "Equalizer-G2:lv2_audio_out_1",
        "input": "Mixer-L:In 2"
      },
      {
        "output": "Equalizer-G2:lv2_audio_out_2",
        "input": "Mixer-R:In 2"
      },
      {
        "output": "Equalizer-G3:lv2_audio_out_1",
        "input": "Mixer-L:In 3"
      },
      {
        "output": "Equalizer-G3:lv2_audio_out_2",
        "input": "Mixer-R:In 3"
      },
      {
        "output": "Equalizer-G4:lv2_audio_out_1",
        "input": "Mixer-L:In 4"
      },
      {
        "output": "Equalizer-G4:lv2_audio_out_2",
        "input": "Mixer-R:In 4"
      }
    ],
    "inputs": [
      "Compressor-G1:in_l",
      "Compressor-G1:in_r"
      "Compressor-G2:in_l",
      "Compressor-G2:in_r"
      "Compressor-G3:in_l",
      "Compressor-G3:in_r"
      "Compressor-G4:in_l",
      "Compressor-G4:in_r"
    ],
    "outputs": [
      "Mixer-L:Out",
      "Mixer-R:Out"
    ]
  },
  "capture.props": {
    "node.name": "pmx-group-channels-{layer_id_lc}-ins",
    "node.autoconnect": false
  },
  "playback.props": {
    "node.name": "pmx-group-channels-{layer_id_lc}-outs",
    "node.autoconnect": false
  }
}
]]

function pmx_filter_builder.create_group_channels_filter_chain(layer_id, local_module)
	print("Building group channels")
	local definition = string.gsub(group_channel_filter_graph_definition, "{layer_id}", layer_id)
	definition = string.gsub(definition, "{layer_id_lc}", string.lower(layer_id))
	return local_module("libpipewire-module-filter-chain", definition, {})
end

local input_channels_filter_graph_definition = [[
{
  "media.name": "Pmx/Input",
  "node.description": "PMX Input Channels",
  "node.autoconnect": false,
  "filter.graph": {
    "nodes": [
      {
        "name": "Saturator-1",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-1",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-1",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-2",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-2",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-2",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-3",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-3",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-3",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-4",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-4",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-4",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-5",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-5",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-5",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-6",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-6",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-6",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-7",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-7",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-7",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-8",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-8",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-8",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-9",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-9",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-9",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-10",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-10",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-10",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-11",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-11",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-11",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-12",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-12",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-12",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-13",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-13",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-13",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-14",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-14",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-14",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-15",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-15",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-15",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-16",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-16",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-16",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-17",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-17",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-17",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-18",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-18",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-18",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-19",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-19",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-19",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-20",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-20",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-20",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-21",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-21",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-21",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-22",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-22",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-22",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-23",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-23",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-23",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-24",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-24",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-24",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-25",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-25",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-25",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-26",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-26",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-26",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-27",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-27",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-27",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-28",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-28",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-28",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-29",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-29",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-29",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-30",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-30",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-30",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-31",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-31",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-31",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Saturator-32",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "control": {}
      },
      {
        "name": "Compressor-32",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "control": {}
      },
      {
        "name": "Equalizer-32",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "control": {}
      },
      {
        "name": "Copy-1",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-2",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-3",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-4",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-5",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-6",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-7",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-8",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-9",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-10",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-11",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-12",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-13",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-14",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-15",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-16",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-17",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-18",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-19",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-20",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-21",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-22",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-23",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-24",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-25",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-26",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-27",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-28",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-29",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-30",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-31",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
      {
        "name": "Copy-32",
        "type": "builtin",
        "label": "copy",
        "control": {}
      },
    ],
    "links": [
      {
        "output": "Copy-1:Out",
        "input": "Saturator-1:in_l"
      },
      {
        "output": "Copy-2:Out",
        "input": "Saturator-1:in_r"
      },
      {
        "output": "Copy-3:Out",
        "input": "Saturator-2:in_l"
      },
      {
        "output": "Copy-4:Out",
        "input": "Saturator-2:in_r"
      },
      {
        "output": "Copy-5:Out",
        "input": "Saturator-3:in_l"
      },
      {
        "output": "Copy-6:Out",
        "input": "Saturator-3:in_r"
      },
      {
        "output": "Copy-7:Out",
        "input": "Saturator-4:in_l"
      },
      {
        "output": "Copy-8:Out",
        "input": "Saturator-4:in_r"
      },
      {
        "output": "Copy-9:Out",
        "input": "Saturator-5:in_l"
      },
      {
        "output": "Copy-10:Out",
        "input": "Saturator-5:in_r"
      },
      {
        "output": "Copy-11:Out",
        "input": "Saturator-6:in_l"
      },
      {
        "output": "Copy-12:Out",
        "input": "Saturator-6:in_r"
      },
      {
        "output": "Copy-13:Out",
        "input": "Saturator-7:in_l"
      },
      {
        "output": "Copy-14:Out",
        "input": "Saturator-7:in_r"
      },
      {
        "output": "Copy-15:Out",
        "input": "Saturator-8:in_l"
      },
      {
        "output": "Copy-16:Out",
        "input": "Saturator-8:in_r"
      },
      {
        "output": "Copy-17:Out",
        "input": "Saturator-9:in_l"
      },
      {
        "output": "Copy-18:Out",
        "input": "Saturator-9:in_r"
      },
      {
        "output": "Copy-19:Out",
        "input": "Saturator-10:in_l"
      },
      {
        "output": "Copy-20:Out",
        "input": "Saturator-10:in_r"
      },
      {
        "output": "Copy-21:Out",
        "input": "Saturator-11:in_l"
      },
      {
        "output": "Copy-22:Out",
        "input": "Saturator-11:in_r"
      },
      {
        "output": "Copy-23:Out",
        "input": "Saturator-12:in_l"
      },
      {
        "output": "Copy-24:Out",
        "input": "Saturator-12:in_r"
      },
      {
        "output": "Copy-25:Out",
        "input": "Saturator-13:in_l"
      },
      {
        "output": "Copy-26:Out",
        "input": "Saturator-13:in_r"
      },
      {
        "output": "Copy-27:Out",
        "input": "Saturator-14:in_l"
      },
      {
        "output": "Copy-28:Out",
        "input": "Saturator-14:in_r"
      },
      {
        "output": "Copy-29:Out",
        "input": "Saturator-15:in_l"
      },
      {
        "output": "Copy-30:Out",
        "input": "Saturator-15:in_r"
      },
      {
        "output": "Copy-31:Out",
        "input": "Saturator-16:in_l"
      },
      {
        "output": "Copy-32:Out",
        "input": "Saturator-16:in_r"
      },
      {
        "output": "Saturator-1:out_l",
        "input": "Compressor-1:in_l"
      },
      {
        "output": "Saturator-1:out_r",
        "input": "Compressor-1:in_r"
      },
      {
        "output": "Compressor-1:out_l",
        "input": "Equalizer-1:lv2_audio_in_1"
      },
      {
        "output": "Compressor-1:out_r",
        "input": "Equalizer-1:lv2_audio_in_2"
      },
      {
        "output": "Saturator-2:out_l",
        "input": "Compressor-2:in_l"
      },
      {
        "output": "Saturator-2:out_r",
        "input": "Compressor-2:in_r"
      },
      {
        "output": "Compressor-2:out_l",
        "input": "Equalizer-2:lv2_audio_in_1"
      },
      {
        "output": "Compressor-2:out_r",
        "input": "Equalizer-2:lv2_audio_in_2"
      },
      {
        "output": "Saturator-3:out_l",
        "input": "Compressor-3:in_l"
      },
      {
        "output": "Saturator-3:out_r",
        "input": "Compressor-3:in_r"
      },
      {
        "output": "Compressor-3:out_l",
        "input": "Equalizer-3:lv2_audio_in_1"
      },
      {
        "output": "Compressor-3:out_r",
        "input": "Equalizer-3:lv2_audio_in_2"
      },
      {
        "output": "Saturator-4:out_l",
        "input": "Compressor-4:in_l"
      },
      {
        "output": "Saturator-4:out_r",
        "input": "Compressor-4:in_r"
      },
      {
        "output": "Compressor-4:out_l",
        "input": "Equalizer-4:lv2_audio_in_1"
      },
      {
        "output": "Compressor-4:out_r",
        "input": "Equalizer-4:lv2_audio_in_2"
      },
      {
        "output": "Saturator-5:out_l",
        "input": "Compressor-5:in_l"
      },
      {
        "output": "Saturator-5:out_r",
        "input": "Compressor-5:in_r"
      },
      {
        "output": "Compressor-5:out_l",
        "input": "Equalizer-5:lv2_audio_in_1"
      },
      {
        "output": "Compressor-5:out_r",
        "input": "Equalizer-5:lv2_audio_in_2"
      },
      {
        "output": "Saturator-6:out_l",
        "input": "Compressor-6:in_l"
      },
      {
        "output": "Saturator-6:out_r",
        "input": "Compressor-6:in_r"
      },
      {
        "output": "Compressor-6:out_l",
        "input": "Equalizer-6:lv2_audio_in_1"
      },
      {
        "output": "Compressor-6:out_r",
        "input": "Equalizer-6:lv2_audio_in_2"
      },
      {
        "output": "Saturator-7:out_l",
        "input": "Compressor-7:in_l"
      },
      {
        "output": "Saturator-7:out_r",
        "input": "Compressor-7:in_r"
      },
      {
        "output": "Compressor-7:out_l",
        "input": "Equalizer-7:lv2_audio_in_1"
      },
      {
        "output": "Compressor-7:out_r",
        "input": "Equalizer-7:lv2_audio_in_2"
      },
      {
        "output": "Saturator-8:out_l",
        "input": "Compressor-8:in_l"
      },
      {
        "output": "Saturator-8:out_r",
        "input": "Compressor-8:in_r"
      },
      {
        "output": "Compressor-8:out_l",
        "input": "Equalizer-8:lv2_audio_in_1"
      },
      {
        "output": "Compressor-8:out_r",
        "input": "Equalizer-8:lv2_audio_in_2"
      },
      {
        "output": "Saturator-9:out_l",
        "input": "Compressor-9:in_l"
      },
      {
        "output": "Saturator-9:out_r",
        "input": "Compressor-9:in_r"
      },
      {
        "output": "Compressor-9:out_l",
        "input": "Equalizer-9:lv2_audio_in_1"
      },
      {
        "output": "Compressor-9:out_r",
        "input": "Equalizer-9:lv2_audio_in_2"
      },
      {
        "output": "Saturator-10:out_l",
        "input": "Compressor-10:in_l"
      },
      {
        "output": "Saturator-10:out_r",
        "input": "Compressor-10:in_r"
      },
      {
        "output": "Compressor-10:out_l",
        "input": "Equalizer-10:lv2_audio_in_1"
      },
      {
        "output": "Compressor-10:out_r",
        "input": "Equalizer-10:lv2_audio_in_2"
      },
      {
        "output": "Saturator-11:out_l",
        "input": "Compressor-11:in_l"
      },
      {
        "output": "Saturator-11:out_r",
        "input": "Compressor-11:in_r"
      },
      {
        "output": "Compressor-11:out_l",
        "input": "Equalizer-11:lv2_audio_in_1"
      },
      {
        "output": "Compressor-11:out_r",
        "input": "Equalizer-11:lv2_audio_in_2"
      },
      {
        "output": "Saturator-12:out_l",
        "input": "Compressor-12:in_l"
      },
      {
        "output": "Saturator-12:out_r",
        "input": "Compressor-12:in_r"
      },
      {
        "output": "Compressor-12:out_l",
        "input": "Equalizer-12:lv2_audio_in_1"
      },
      {
        "output": "Compressor-12:out_r",
        "input": "Equalizer-12:lv2_audio_in_2"
      },
      {
        "output": "Saturator-13:out_l",
        "input": "Compressor-13:in_l"
      },
      {
        "output": "Saturator-13:out_r",
        "input": "Compressor-13:in_r"
      },
      {
        "output": "Compressor-13:out_l",
        "input": "Equalizer-13:lv2_audio_in_1"
      },
      {
        "output": "Compressor-13:out_r",
        "input": "Equalizer-13:lv2_audio_in_2"
      },
      {
        "output": "Saturator-14:out_l",
        "input": "Compressor-14:in_l"
      },
      {
        "output": "Saturator-14:out_r",
        "input": "Compressor-14:in_r"
      },
      {
        "output": "Compressor-14:out_l",
        "input": "Equalizer-14:lv2_audio_in_1"
      },
      {
        "output": "Compressor-14:out_r",
        "input": "Equalizer-14:lv2_audio_in_2"
      },
      {
        "output": "Saturator-15:out_l",
        "input": "Compressor-15:in_l"
      },
      {
        "output": "Saturator-15:out_r",
        "input": "Compressor-15:in_r"
      },
      {
        "output": "Compressor-15:out_l",
        "input": "Equalizer-15:lv2_audio_in_1"
      },
      {
        "output": "Compressor-15:out_r",
        "input": "Equalizer-15:lv2_audio_in_2"
      },
      {
        "output": "Saturator-16:out_l",
        "input": "Compressor-16:in_l"
      },
      {
        "output": "Saturator-16:out_r",
        "input": "Compressor-16:in_r"
      },
      {
        "output": "Compressor-16:out_l",
        "input": "Equalizer-16:lv2_audio_in_1"
      },
      {
        "output": "Compressor-16:out_r",
        "input": "Equalizer-16:lv2_audio_in_2"
      },
      {
        "output": "Saturator-17:out_l",
        "input": "Compressor-17:in_l"
      },
      {
        "output": "Saturator-17:out_r",
        "input": "Compressor-17:in_r"
      },
      {
        "output": "Compressor-17:out_l",
        "input": "Equalizer-17:lv2_audio_in_1"
      },
      {
        "output": "Compressor-17:out_r",
        "input": "Equalizer-17:lv2_audio_in_2"
      },
      {
        "output": "Saturator-18:out_l",
        "input": "Compressor-18:in_l"
      },
      {
        "output": "Saturator-18:out_r",
        "input": "Compressor-18:in_r"
      },
      {
        "output": "Compressor-18:out_l",
        "input": "Equalizer-18:lv2_audio_in_1"
      },
      {
        "output": "Compressor-18:out_r",
        "input": "Equalizer-18:lv2_audio_in_2"
      },
      {
        "output": "Saturator-19:out_l",
        "input": "Compressor-19:in_l"
      },
      {
        "output": "Saturator-19:out_r",
        "input": "Compressor-19:in_r"
      },
      {
        "output": "Compressor-19:out_l",
        "input": "Equalizer-19:lv2_audio_in_1"
      },
      {
        "output": "Compressor-19:out_r",
        "input": "Equalizer-19:lv2_audio_in_2"
      },
      {
        "output": "Saturator-20:out_l",
        "input": "Compressor-20:in_l"
      },
      {
        "output": "Saturator-20:out_r",
        "input": "Compressor-20:in_r"
      },
      {
        "output": "Compressor-20:out_l",
        "input": "Equalizer-20:lv2_audio_in_1"
      },
      {
        "output": "Compressor-20:out_r",
        "input": "Equalizer-20:lv2_audio_in_2"
      },
      {
        "output": "Saturator-21:out_l",
        "input": "Compressor-21:in_l"
      },
      {
        "output": "Saturator-21:out_r",
        "input": "Compressor-21:in_r"
      },
      {
        "output": "Compressor-21:out_l",
        "input": "Equalizer-21:lv2_audio_in_1"
      },
      {
        "output": "Compressor-21:out_r",
        "input": "Equalizer-21:lv2_audio_in_2"
      },
      {
        "output": "Saturator-22:out_l",
        "input": "Compressor-22:in_l"
      },
      {
        "output": "Saturator-22:out_r",
        "input": "Compressor-22:in_r"
      },
      {
        "output": "Compressor-22:out_l",
        "input": "Equalizer-22:lv2_audio_in_1"
      },
      {
        "output": "Compressor-22:out_r",
        "input": "Equalizer-22:lv2_audio_in_2"
      },
      {
        "output": "Saturator-23:out_l",
        "input": "Compressor-23:in_l"
      },
      {
        "output": "Saturator-23:out_r",
        "input": "Compressor-23:in_r"
      },
      {
        "output": "Compressor-23:out_l",
        "input": "Equalizer-23:lv2_audio_in_1"
      },
      {
        "output": "Compressor-23:out_r",
        "input": "Equalizer-23:lv2_audio_in_2"
      },
      {
        "output": "Saturator-24:out_l",
        "input": "Compressor-24:in_l"
      },
      {
        "output": "Saturator-24:out_r",
        "input": "Compressor-24:in_r"
      },
      {
        "output": "Compressor-24:out_l",
        "input": "Equalizer-24:lv2_audio_in_1"
      },
      {
        "output": "Compressor-24:out_r",
        "input": "Equalizer-24:lv2_audio_in_2"
      },
      {
        "output": "Saturator-25:out_l",
        "input": "Compressor-25:in_l"
      },
      {
        "output": "Saturator-25:out_r",
        "input": "Compressor-25:in_r"
      },
      {
        "output": "Compressor-25:out_l",
        "input": "Equalizer-25:lv2_audio_in_1"
      },
      {
        "output": "Compressor-25:out_r",
        "input": "Equalizer-25:lv2_audio_in_2"
      },
      {
        "output": "Saturator-26:out_l",
        "input": "Compressor-26:in_l"
      },
      {
        "output": "Saturator-26:out_r",
        "input": "Compressor-26:in_r"
      },
      {
        "output": "Compressor-26:out_l",
        "input": "Equalizer-26:lv2_audio_in_1"
      },
      {
        "output": "Compressor-26:out_r",
        "input": "Equalizer-26:lv2_audio_in_2"
      },
      {
        "output": "Saturator-27:out_l",
        "input": "Compressor-27:in_l"
      },
      {
        "output": "Saturator-27:out_r",
        "input": "Compressor-27:in_r"
      },
      {
        "output": "Compressor-27:out_l",
        "input": "Equalizer-27:lv2_audio_in_1"
      },
      {
        "output": "Compressor-27:out_r",
        "input": "Equalizer-27:lv2_audio_in_2"
      },
      {
        "output": "Saturator-28:out_l",
        "input": "Compressor-28:in_l"
      },
      {
        "output": "Saturator-28:out_r",
        "input": "Compressor-28:in_r"
      },
      {
        "output": "Compressor-28:out_l",
        "input": "Equalizer-28:lv2_audio_in_1"
      },
      {
        "output": "Compressor-28:out_r",
        "input": "Equalizer-28:lv2_audio_in_2"
      },
      {
        "output": "Saturator-29:out_l",
        "input": "Compressor-29:in_l"
      },
      {
        "output": "Saturator-29:out_r",
        "input": "Compressor-29:in_r"
      },
      {
        "output": "Compressor-29:out_l",
        "input": "Equalizer-29:lv2_audio_in_1"
      },
      {
        "output": "Compressor-29:out_r",
        "input": "Equalizer-29:lv2_audio_in_2"
      },
      {
        "output": "Saturator-30:out_l",
        "input": "Compressor-30:in_l"
      },
      {
        "output": "Saturator-30:out_r",
        "input": "Compressor-30:in_r"
      },
      {
        "output": "Compressor-30:out_l",
        "input": "Equalizer-30:lv2_audio_in_1"
      },
      {
        "output": "Compressor-30:out_r",
        "input": "Equalizer-30:lv2_audio_in_2"
      },
      {
        "output": "Saturator-31:out_l",
        "input": "Compressor-31:in_l"
      },
      {
        "output": "Saturator-31:out_r",
        "input": "Compressor-31:in_r"
      },
      {
        "output": "Compressor-31:out_l",
        "input": "Equalizer-31:lv2_audio_in_1"
      },
      {
        "output": "Compressor-31:out_r",
        "input": "Equalizer-31:lv2_audio_in_2"
      },
      {
        "output": "Saturator-32:out_l",
        "input": "Compressor-32:in_l"
      },
      {
        "output": "Saturator-32:out_r",
        "input": "Compressor-32:in_r"
      },
      {
        "output": "Compressor-32:out_l",
        "input": "Equalizer-32:lv2_audio_in_1"
      },
      {
        "output": "Compressor-32:out_r",
        "input": "Equalizer-32:lv2_audio_in_2"
      }
    ],
    "inputs": [
      "Copy-1:In",
      "Copy-2:In",
      "Copy-3:In",
      "Copy-4:In",
      "Copy-5:In",
      "Copy-6:In",
      "Copy-7:In",
      "Copy-8:In",
      "Copy-9:In",
      "Copy-10:In",
      "Copy-11:In",
      "Copy-12:In",
      "Copy-13:In",
      "Copy-14:In",
      "Copy-15:In",
      "Copy-16:In",
      "Copy-17:In",
      "Copy-18:In",
      "Copy-19:In",
      "Copy-20:In",
      "Copy-21:In",
      "Copy-22:In",
      "Copy-23:In",
      "Copy-24:In",
      "Copy-25:In",
      "Copy-26:In",
      "Copy-27:In",
      "Copy-28:In",
      "Copy-29:In",
      "Copy-30:In",
      "Copy-31:In",
      "Copy-32:In"
    ],
    "outputs": [
      "Equalizer-1:lv2_audio_out_1",
      "Equalizer-1:lv2_audio_out_2",
      "Equalizer-2:lv2_audio_out_1",
      "Equalizer-2:lv2_audio_out_2",
      "Equalizer-3:lv2_audio_out_1",
      "Equalizer-3:lv2_audio_out_2",
      "Equalizer-4:lv2_audio_out_1",
      "Equalizer-4:lv2_audio_out_2",
      "Equalizer-5:lv2_audio_out_1",
      "Equalizer-5:lv2_audio_out_2",
      "Equalizer-6:lv2_audio_out_1",
      "Equalizer-6:lv2_audio_out_2",
      "Equalizer-7:lv2_audio_out_1",
      "Equalizer-7:lv2_audio_out_2",
      "Equalizer-8:lv2_audio_out_1",
      "Equalizer-8:lv2_audio_out_2",
      "Equalizer-9:lv2_audio_out_1",
      "Equalizer-9:lv2_audio_out_2",
      "Equalizer-10:lv2_audio_out_1",
      "Equalizer-10:lv2_audio_out_2",
      "Equalizer-11:lv2_audio_out_1",
      "Equalizer-11:lv2_audio_out_2",
      "Equalizer-12:lv2_audio_out_1",
      "Equalizer-12:lv2_audio_out_2",
      "Equalizer-13:lv2_audio_out_1",
      "Equalizer-13:lv2_audio_out_2",
      "Equalizer-14:lv2_audio_out_1",
      "Equalizer-14:lv2_audio_out_2",
      "Equalizer-15:lv2_audio_out_1",
      "Equalizer-15:lv2_audio_out_2",
      "Equalizer-16:lv2_audio_out_1",
      "Equalizer-16:lv2_audio_out_2",
      "Equalizer-17:lv2_audio_out_1",
      "Equalizer-17:lv2_audio_out_2",
      "Equalizer-18:lv2_audio_out_1",
      "Equalizer-18:lv2_audio_out_2",
      "Equalizer-19:lv2_audio_out_1",
      "Equalizer-19:lv2_audio_out_2",
      "Equalizer-20:lv2_audio_out_1",
      "Equalizer-20:lv2_audio_out_2",
      "Equalizer-21:lv2_audio_out_1",
      "Equalizer-21:lv2_audio_out_2",
      "Equalizer-22:lv2_audio_out_1",
      "Equalizer-22:lv2_audio_out_2",
      "Equalizer-23:lv2_audio_out_1",
      "Equalizer-23:lv2_audio_out_2",
      "Equalizer-24:lv2_audio_out_1",
      "Equalizer-24:lv2_audio_out_2",
      "Equalizer-25:lv2_audio_out_1",
      "Equalizer-25:lv2_audio_out_2",
      "Equalizer-26:lv2_audio_out_1",
      "Equalizer-26:lv2_audio_out_2",
      "Equalizer-27:lv2_audio_out_1",
      "Equalizer-27:lv2_audio_out_2",
      "Equalizer-28:lv2_audio_out_1",
      "Equalizer-28:lv2_audio_out_2",
      "Equalizer-29:lv2_audio_out_1",
      "Equalizer-29:lv2_audio_out_2",
      "Equalizer-30:lv2_audio_out_1",
      "Equalizer-30:lv2_audio_out_2",
      "Equalizer-31:lv2_audio_out_1",
      "Equalizer-31:lv2_audio_out_2",
      "Equalizer-32:lv2_audio_out_1",
      "Equalizer-32:lv2_audio_out_2"
    ]
  },
  "capture.props": {
    "node.name": "pmx-input-channels-ins",
    "node.autoconnect": false
  },
  "playback.props": {
    "node.name": "pmx-input-channels-outs",
    "node.autoconnect": false
  }
}
]]

function pmx_filter_builder.build_input_channels(local_module)
	print("Building input channels")
	return local_module("libpipewire-module-filter-chain", input_channels_filter_graph_definition, {})
end

return pmx_filter_builder