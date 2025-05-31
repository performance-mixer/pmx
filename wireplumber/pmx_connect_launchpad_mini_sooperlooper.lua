SlpCtrlOutputPorts = {}
SlpCtrlInputPorts = {}
LpMiniMk3OutputPorts = {}
LpMiniMk3InputPorts = {}

local function connect_slp_ctrl_output_to_mk3_input()
    for i = 1, #SlpCtrlOutputPorts do
        for j = 1, #LpMiniMk3InputPorts do
            local output_port_id = SlpCtrlOutputPorts[i].properties["object.id"]
            local input_port_id = LpMiniMk3InputPorts[j].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
end

local function connect_slp_ctrl_input_to_mk3_output()
    for i = 1, #SlpCtrlInputPorts do
        for j = 1, #LpMiniMk3OutputPorts do
            local input_port_id = SlpCtrlInputPorts[i].properties["object.id"]
            local output_port_id = LpMiniMk3OutputPorts[j].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
end

SimpleEventHook({
    name = "pmx/connect-lp-mini-ports/watch-out-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.alias", "=", "pmx-slp-ctrl:lp_mini" }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        new_output_port = event:get_subject()
        print("found", new_output_port.properties["port.alias"])
        table.insert(SlpCtrlOutputPorts, new_output_port)
        connect_slp_ctrl_output_to_mk3_input()
    end
}):register()

SimpleEventHook({
    name = "pmx/connect-lp-mini-ports/watch-in-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.alias", "=", "pmx-slp-ctrl:lp_mini" }),
            Constraint({ "port.direction", "=", "in" })
        })
    },
    execute = function(event)
        new_input_port = event:get_subject()
        print("found", new_input_port.properties["port.alias"])
        table.insert(SlpCtrlInputPorts, new_input_port)
        connect_slp_ctrl_input_to_mk3_output()
    end
}):register()

SimpleEventHook({
    name = "pmx/connect-lp-mini-mk3-ports/watch-out-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.alias", "=", "Launchpad Mini MK3:Launchpad Mini MK3 LPMiniMK3 DA" }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        new_output_port = event:get_subject()
        print("found", new_output_port.properties["port.alias"])
        table.insert(LpMiniMk3OutputPorts, new_output_port)
        connect_slp_ctrl_input_to_mk3_output()
    end
}):register()

SimpleEventHook({
    name = "pmx/connect-lp-mini-mk3-ports/watch-in-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.alias", "=", "Launchpad Mini MK3:Launchpad Mini MK3 LPMiniMK3 DA" }),
            Constraint({ "port.direction", "=", "in" })
        })
    },
    execute = function(event)
        new_input_port = event:get_subject()
        print("found", new_input_port.properties["port.alias"])
        table.insert(LpMiniMk3InputPorts, new_input_port)
        connect_slp_ctrl_output_to_mk3_input()
    end
}):register()

