function connect_all(source_port, target_ports)
    if source_port == nil then
        return
    end
    for _, target_port in ipairs(target_ports) do
        local link = Link("link-factory", {
            ["link.output.port"] = source_port.properties["object.id"],
            ["link.input.port"] = target_port.properties["object.id"],
            ["object.linger"] = true
        })
        link:activate(1)
    end
end

HapaxOutPort = {}
HapaxTargetPorts = {}

SimpleEventHook({
    name = "Find Hapax",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.alias", "=",
                         "HAPAX:HAPAX MIDI 1",
            }),
        })

    },
    execute = function(event)
        HapaxOutPort = event:get_subject()
        connect_all(HapaxOutPort, HapaxTargetPorts)
    end
}):register()

MidiClockOutputPort = {}
MidiClockTargetPorts = {}

SimpleEventHook({
    name = "Find Midi Clock Port",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.alias", "=",
                         "jack_midi_clock:mclk_out"
            }),
        })
    },
    execute = function(event)
        MidiClockOutputPort = event:get_subject()
        connect_all(MidiClockOutputPort, MidiClockTargetPorts)
    end
}):register()

SimpleEventHook({
    name = "Hapax Targets",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "in" }),
            Constraint({ "port.alias", "c",
                         "Prophet Rev2 Module:Prophet Rev2 Module MIDI 1",
                         "COBALT8:COBALT8 MIDI 1",
                         "Modor NF-1:Modor NF-1 MIDI 1",
                         "SE-02:SE-02 MIDI 1",
                         "SYSTEM-1m:SYSTEM-1m MIDI 1",
            }),
        })
    },
    execute = function(event)
        local port = event:get_subject()
        table.insert(HapaxTargetPorts, port)
        connect_all(HapaxOutPort, HapaxTargetPorts)
    end
}):register()

SimpleEventHook({
    name = "Midi Clock Targets",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "in" }),
            Constraint({ "port.alias", "c",
                         "Prophet Rev2 Module:Prophet Rev2 Module MIDI 1",
                         "COBALT8:COBALT8 MIDI 1",
                         "HAPAX:HAPAX MIDI 1",
                         "Modor NF-1:Modor NF-1 MIDI 1",
                         "SE-02:SE-02 MIDI 1",
                         "RME RayDAT_a5963e:HDSPMxa5963e MIDI 1",
                         "SYSTEM-1m:SYSTEM-1m MIDI 1",
                         "pmx-slp-ctrl:midi_clock",
                         "sooperlooper:sooperlooper"
            }),
        })
    },
    execute = function(event)
        local port = event:get_subject()
        table.insert(MidiClockTargetPorts, port)
        connect_all(MidiClockOutputPort, MidiClockTargetPorts)
    end
}):register()