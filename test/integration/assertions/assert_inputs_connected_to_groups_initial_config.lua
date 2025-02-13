local object_manager = ObjectManager({
    Interest({ type = "link" }),
    Interest({ type = "node" })
})

object_manager:activate()

object_manager:connect("installed", function(om)
    local group_a = object_manager:lookup(
            Interest({
                type = "node",
                Constraint({ "node.name", "=", "pmx-group-channels-a-ins" })
            }))
    local group_b = object_manager:lookup(
            Interest({
                type = "node",
                Constraint({ "node.name", "=", "pmx-group-channels-b-ins" })
            }))
    local inputs = object_manager:lookup(
            Interest({
                type = "node",
                Constraint({ "node.name", "=", "pmx-input-channels-outs" })
            }))

    if group_a == nil or group_b == nil or inputs == nil then
        print("not all nodes set up correctly")
        error("no all nodes set up correctly")
    end

    group_port_a_l = group_a:lookup_port {
        Constraint { "port.direction", "=", "in" },
        Constraint { "port.id", "=", tostring(0) },
    }

    group_port_a_r = group_a:lookup_port {
        Constraint { "port.direction", "=", "in" },
        Constraint { "port.id", "=", tostring(1) },
    }

    group_port_b_l = group_b:lookup_port {
        Constraint { "port.direction", "=", "in" },
        Constraint { "port.id", "=", tostring(0) },
    }

    group_port_b_r = group_b:lookup_port {
        Constraint { "port.direction", "=", "in" },
        Constraint { "port.id", "=", tostring(1) },
    }

    for input_ports_id = 0, 31 do
        local inputs_port_a = inputs:lookup_port {
            Constraint { "port.direction", "=", "out" },
            Constraint { "port.id", "=", tostring(input_ports_id) },
        }

        local target_port_a = {}
        local target_port_b = {}
        if input_ports_id % 2 == 0 then
            target_port_a = group_port_a_l
            target_port_b = group_port_b_l
        else
            target_port_a = group_port_a_r
            target_port_b = group_port_b_r
        end

        print("lookup link a for channel " .. input_ports_id)
        local link_a = object_manager:lookup(Interest({
            type = "link",
            Constraint({ "link.output.node", "=", inputs_port_a.properties["node.id"] }),
            Constraint({ "link.output.port", "=", inputs_port_a.properties["object.id"] }),
            Constraint({ "link.input.node", "=", target_port_a.properties["node.id"] }),
            Constraint({ "link.input.port", "=", target_port_a.properties["object.id"] }),
        }))
        if link_a == nil then
            print("couldn't find link a")
            print("Link " ..
                    inputs_port_a.properties["node.id"] .. "," ..
                    inputs_port_a.properties["object.id"] .. " -> " ..
                    target_port_a.properties["node.id"] .. "," ..
                    target_port_a.properties["object.id"])
        end

        local inputs_port_b = inputs:lookup_port {
            Constraint { "port.direction", "=", "out" },
            Constraint { "port.id", "=", tostring(32 + input_ports_id) },
        }

        print("lookup link b for channel " .. input_ports_id)
        local link_b = object_manager:lookup(Interest({
            type = "link",
            Constraint({ "link.output.node", "=", inputs_port_b.properties["node.id"] }),
            Constraint({ "link.output.port", "=", inputs_port_b.properties["object.id"] }),
            Constraint({ "link.input.node", "=", target_port_b.properties["node.id"] }),
            Constraint({ "link.input.port", "=", target_port_b.properties["object.id"] }),
        }))
        if link_b == nil then
            print("couldn't find link b")
            print("Link " ..
                    inputs_port_b.properties["node.id"] .. "," ..
                    inputs_port_b.properties["object.id"] .. " -> " ..
                    target_port_b.properties["node.id"] .. "," ..
                    target_port_b.properties["object.id"])
        end
    end
end)
