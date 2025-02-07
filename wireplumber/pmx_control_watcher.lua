local pmx_utils = require("pmx_utils")

local filter_chain_ctrl = nil
local midi_router = nil
local network_receiver = nil
local cmd_mm_1_port_alias = "CMD MM-1:CMD MM-1 MIDI 1"
local cmd_mm_1_port = nil
local fader_fox_port_alias = "Faderfox PC4:Faderfox PC4 MIDI 1";
local fader_fox_port = nil
local traktor_z1_router = nil
local network_sender = nil

SimpleEventHook({
    name = "pmx/control_watcher/filter_chain_ctrl",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-filter-chain-ctrl" })
        })
    },
    execute = function(event)
        filter_chain_ctrl = event:get_subject()
        pmx_utils.raise_connect_event_port_to_node(network_receiver,
                filter_chain_ctrl)
        pmx_utils.raise_connect_event(traktor_z1_router, "0",
                filter_chain_ctrl, "0");
        pmx_utils.raise_connect_event(midi_router, "0", filter_chain_ctrl, "0")
    end,
}):register()

SimpleEventHook({
    name = "pmx/control_watcher/pmx-osc-network-receiver",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-osc-network-receiver" })
        })
    },
    execute = function(event)
        network_receiver = event:get_subject()
        pmx_utils.raise_connect_event_port_to_node(network_receiver,
                filter_chain_ctrl)
    end,
}):register()

SimpleEventHook({
    name = "pmx/control_watcher/pmx-osc-network-sender",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-osc-network-sender" })
        })
    },
    execute = function(event)
        network_sender = event:get_subject()
        pmx_utils.raise_connect_event_port_to_node(traktor_z1_router,
                network_sender)
        pmx_utils.raise_connect_event_port_to_node(midi_router, network_sender)
    end,
}):register()

SimpleEventHook({
    name = "pmx/control_watcher/pmx-traktor-z1-router",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-traktor-z1-router" })
        })
    },
    execute = function(event)
        traktor_z1_router = event:get_subject()
        pmx_utils.raise_connect_event(traktor_z1_router, "0",
                network_sender, "0");
        pmx_utils.raise_connect_event(traktor_z1_router, "0",
                filter_chain_ctrl, "0");
    end,
}):register()

SimpleEventHook({
    name = "pmx/control_watcher/pmx-midi-router",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-midi-router" })
        })
    },
    execute = function(event)
        midi_router = event:get_subject()
        pmx_utils.raise_connect_event(midi_router, "0", network_sender, "0")
        pmx_utils.raise_connect_event(midi_router, "0", filter_chain_ctrl, "0")
        pmx_utils.raise_connect_event_port_to_node(cmd_mm_1_port, midi_router,
                "1")
        pmx_utils.raise_connect_event_port_to_node(fader_fox_port, midi_router,
                "0")
    end,
}):register()

SimpleEventHook({
    name = "pmx/control_watcher/faderfox_port",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.alias", "=", fader_fox_port_alias }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        fader_fox_port = event:get_subject()
        pmx_utils.raise_connect_event_port_to_node(fader_fox_port, midi_router,
                "0")
    end,
}):register()

SimpleEventHook({
    name = "pmx/control_watcher/cmd_mm_1_port",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.alias", "=", cmd_mm_1_port_alias }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        cmd_mm_1_port = event:get_subject()
        pmx_utils.raise_connect_event_port_to_node(cmd_mm_1_port, midi_router,
                "1")
    end,
}):register()