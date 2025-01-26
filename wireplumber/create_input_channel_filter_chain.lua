local filter_definition = [[
{
  "audio.channels": 2,
  "media.name": "Pmx/Input/{channel_id}",
  "node.description": "PMX Input Channel Strip {channel_id}",
  "node.autoconnect": false,
  "filter.graph": {
    "nodes": [
      {
        "name": "Saturator",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Saturator",
        "label": "Saturator",
        "control": {}
      },
      {
        "name": "Compressor",
        "type": "lv2",
        "plugin": "http://calf.sourceforge.net/plugins/Compressor",
        "label": "Compressor",
        "control": {}
      },
      {
        "name": "Equalizer",
        "type": "lv2",
        "plugin": "http://distrho.sf.net/plugins/3BandEQ",
        "label": "Equalizer",
        "control": {}
      }
    ],
    "links": [
      {
        "output": "Saturator:out_l",
        "input": "Compressor:in_l"
      },
      {
        "output": "Saturator:out_r",
        "input": "Compressor:in_r"
      },
      {
        "output": "Compressor:out_l",
        "input": "Equalizer:lv2_audio_in_1"
      },
      {
        "output": "Compressor:out_r",
        "input": "Equalizer:lv2_audio_in_2"
      }
    ],
    "inputs": [
      "Saturator:in_l",
      "Saturator:in_r"
    ],
    "outputs": [
      "Equalizer:lv2_audio_out_1",
      "Equalizer:lv2_audio_out_2"
    ]
  },
  "capture.props": {
    "node.name": "pmx channel strip {channel_id}",
    "node.autoconnect": false
  },
  "playback.props": {
    "node.name": "pmx channel strip {channel_id}",
    "node.autoconnect": false
  }
}
]]

local function create_filter_chain(channel_id)
	local definition = string.gsub(filter_definition, "{channel_id}", channel_id)
	return LocalModule("libpipewire-module-filter-chain", definition, {})
end

local filter_chain = create_filter_chain("01")
