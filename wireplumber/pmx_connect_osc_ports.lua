output_ports = {}
input_ports = {}

SimpleEventHook({
    name = "pmx/connect-osc-ports/watch-out-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "=", "pmx-osc" }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        new_output_port = event:get_subject()
        table.insert(output_ports, new_output_port)
        local output_port_id = new_output_port.properties["object.id"]

        for i = 1, #input_ports do
            local input_port_id = input_ports[i].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

SimpleEventHook({
    name = "pmx/connect-osc-ports/watch-in-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "=", "pmx-osc" }),
            Constraint({ "port.direction", "=", "in" })
        })
    },
    execute = function(event)
        new_input_port = event:get_subject()
        table.insert(input_ports, new_input_port)
        local input_port_id = new_input_port.properties["object.id"]

        for i = 1, #output_ports do
            local output_port_id = output_ports[i].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

FromSlOscOutputPorts = {}
FromSlOscInputPort = {}

SimpleEventHook({
    name = "pmx/connect-sl-osc-ports/watch-from-out-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "=", "pmx-from-sl-osc" }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        new_output_port = event:get_subject()
        print("found", new_input_port.properties["port.alias"])
        table.insert(FromSlOscOutputPorts, new_output_port)
        local output_port_id = new_output_port.properties["object.id"]

        for i = 1, #FromSlOscInputPort do
            local input_port_id = FromSlOscInputPort[i].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

SimpleEventHook({
    name = "pmx/connect-sl-osc-ports/watch-from-in-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "=", "pmx-from-sl-osc" }),
            Constraint({ "port.direction", "=", "in" })
        })
    },
    execute = function(event)
        new_input_port = event:get_subject()
        print("found", new_input_port.properties["port.alias"])
        table.insert(FromSlOscInputPort, new_input_port)
        local input_port_id = new_input_port.properties["object.id"]

        for i = 1, #FromSlOscOutputPorts do
            local output_port_id = FromSlOscOutputPorts[i].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

ToSlOscOutputPorts = {}
ToSlOscInputPort = {}

SimpleEventHook({
    name = "pmx/connect-sl-osc-ports/watch-to-out-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "=", "pmx-to-sl-osc" }),
            Constraint({ "port.direction", "=", "out" })
        })
    },
    execute = function(event)
        new_output_port = event:get_subject()
        print("found", new_input_port.properties["port.alias"])
        table.insert(ToSlOscOutputPorts, new_output_port)
        local output_port_id = new_output_port.properties["object.id"]

        for i = 1, #ToSlOscInputPort do
            local input_port_id = ToSlOscInputPort[i].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

SimpleEventHook({
    name = "pmx/connect-sl-osc-ports/watch-to-in-ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.name", "=", "pmx-to-sl-osc" }),
            Constraint({ "port.direction", "=", "in" })
        })
    },
    execute = function(event)
        new_input_port = event:get_subject()
        print("found", new_input_port.properties["port.alias"])
        table.insert(ToSlOscInputPort, new_input_port)
        local input_port_id = new_input_port.properties["object.id"]

        for i = 1, #ToSlOscOutputPorts do
            local output_port_id = ToSlOscOutputPorts[i].properties["object.id"]
            local link = Link("link-factory", {
                ["link.output.port"] = output_port_id,
                ["link.input.port"] = input_port_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()
