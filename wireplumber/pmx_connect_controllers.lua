FaderFoxPC4 = nil
BehringerCMD = nil
MidiRouterInputs = nil
MidiRouterGroups = nil

function connect_fader_fox()
    if FaderFoxPC4 ~= nil and MidiRouterInputs ~= nil then
        local link = Link("link-factory", {
            ["link.output.port"] = FaderFoxPC4.properties["object.id"],
            ["link.input.port"] = MidiRouterInputs.properties["object.id"],
            ["object.linger"] = true
        })
        link:activate(1)
    end
end

function connect_b_cmd()
    if BehringerCMD ~= nil and MidiRouterGroups ~= nil then
        local link = Link("link-factory", {
            ["link.output.port"] = BehringerCMD.properties["object.id"],
            ["link.input.port"] = MidiRouterGroups.properties["object.id"],
            ["object.linger"] = true
        })
        link:activate(1)
    end
end

SimpleEventHook({
    name = "inputs_controller",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "#", "*Faderfox PC4*" }),
            Constraint({ "port.direction", "=", "out" }),
        })
    },
    execute = function(event)
        FaderFoxPC4 = event:get_subject()
        connect_fader_fox()
    end
}):register()

SimpleEventHook({
    name = "groups_controller",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "#", "*CMD MM*" }),
            Constraint({ "port.direction", "=", "out" }),
        })
    },
    execute = function(event)
        BehringerCMD = event:get_subject()
        connect_b_cmd()
    end
}):register()

SimpleEventHook({
    name = "midi-router-ff",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "object.path", "=", "pmx-midi-router:playback_0" }),
            Constraint({ "port.direction", "=", "in" }),
        })
    },
    execute = function(event)
        MidiRouterInputs = event:get_subject()
        connect_fader_fox()
    end
}):register()

SimpleEventHook({
    name = "midi-router-cmd",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "object.path", "=", "pmx-midi-router:playback_1" }),
            Constraint({ "port.direction", "=", "in" }),
        })
    },
    execute = function(event)
        MidiRouterGroups = event:get_subject()
        connect_b_cmd()
    end
}):register()
