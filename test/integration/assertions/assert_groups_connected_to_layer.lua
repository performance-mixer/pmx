local object_manager = ObjectManager({
    Interest({ type = "link" }),
    Interest({ type = "node" })
})

object_manager:activate()

object_manager:connect("installed", function(om)
    print("om installed")
    print("lookup group a")
    local group_a = object_manager:lookup(
            Interest({
                type = "node",
                Constraint({ "node.name", "=", "pmx-group-channels-a-outs" })
            }))
    print("lookup group b")
    local group_b = object_manager:lookup(
            Interest({
                type = "node",
                Constraint({ "node.name", "=", "pmx-group-channels-b-outs" })
            }))
    print("lookup layer")
    local layer = object_manager:lookup(
            Interest({
                type = "node",
                Constraint({ "node.name", "=", "pmx-layer-channels-ins" })
            }))

    print("Checking nodes")
    if group_a == nil then
        print("Group A not found: pmx-group-channels-a-outs")
    end

    if group_b == nil then
        print("Group B not found: pmx-group-channels-b-outs")
    end

    if layer == nil then
        print("Layer not found: pmx-layer-channels-ins")
    end

    for group_port_id = 0, 1 do
        print("checking port " .. group_port_id)
        print("lookup port group a")
        local source_port_a = group_a:lookup_port {
            Constraint { "port.direction", "=", "out" },
            Constraint { "port.id", "=", tostring(group_port_id) },
        }
        if source_port_a == nil then
            print("source port a nil")
        end

        print("lookup port group b")
        local source_port_b = group_b:lookup_port({
            Constraint { "port.direction", "=", "out" },
            Constraint { "port.id", "=", tostring(group_port_id) },
        })
        if source_port_b == nil then
            print("source port b nil")
        end

        print("lookup layer a")
        local target_port_a = layer:lookup_port({
            Constraint { "port.direction", "=", "in" },
            Constraint { "port.id", "=", tostring(group_port_id) },
        })
        if target_port_a == nil then
            print("target port a nil", group_port_id)
        end

        print("lookup layer b")
        local target_port_b = layer:lookup_port({
            Constraint { "port.direction", "=", "in" },
            Constraint { "port.id", "=", tostring(group_port_id + 2) },
        })
        if target_port_b == nil then
            print("target port b nil", group_port_id + 2)
        end

        local link_a = object_manager:lookup(Interest({
            type = "link",
            Constraint({ "link.output.node", "=", source_port_a.properties["node.id"] }),
            Constraint({ "link.output.port", "=", source_port_a.properties["object.id"] }),
            Constraint({ "link.input.node", "=", target_port_a.properties["node.id"] }),
            Constraint({ "link.input.port", "=", target_port_a.properties["object.id"] }),
        }))
        if not link_a then
            print("Link " .. source_port_a.properties["node.id"] .. "," .. source_port_a.properties["object.id"] .. "->" ..
                    target_port_a.properties["node.id"] .. ", " .. target_port_a.properties["object.id"])
        end

        print("lookup link b")
        local link_b = object_manager:lookup(Interest({
            type = "link",
            Constraint({ "link.output.node", "=", source_port_b.properties["node.id"] }),
            Constraint({ "link.output.port", "=", source_port_b.properties["object.id"] }),
            Constraint({ "link.input.node", "=", target_port_b.properties["node.id"] }),
            Constraint({ "link.input.port", "=", target_port_b.properties["object.id"] }),
        }))

        if not link_b then
            print("Link " .. source_port_b.properties["node.id"] .. "," .. source_port_b.properties["object.id"] .. "->" ..
                    target_port_b.properties["node.id"] .. ", " .. target_port_b.properties["object.id"])
        end
    end

    print("success: All links created")
end)