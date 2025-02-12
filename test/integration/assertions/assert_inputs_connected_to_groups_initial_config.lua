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

    if group_a ~= nil or group_b ~= nil or inputs ~nil then
        print("no all nodes set up correctly")
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
        local input_port_a = inputs:lookup_port {
            Constraint { "port.direction", "=", "out" },
            Constraint { "port.id", "=", tostring(input_ports_id) },
        }
        local input_port_b = inputs:lookup_port {
            Constraint { "port.direction", "=", "out" },
            Constraint { "port.id", "=", tostring(32 + input_ports_id) },
        }
    end
end)
