pmx_utils = {}

function pmx_utils.raise_connect_event(source_node, source_port_id, target_node,
        target_port_id)
    if source_node ~= nil and target_node ~= nil then
        local source_port = source_node:lookup_port(Interest({
            Constraint({ "port.id", "=", source_port_id }),
            Constraint({ "port.direction", "=", "out" })
        }))
        local target_port = target_node:lookup_port(Interest({
            Constraint({ "port.id", "=", target_port_id }),
            Constraint({ "port.direction", "=", "in" })
        }))
        if source_port ~= nil and target_port ~= nil then
            source:call("push-event", "pmx_connect_ports", source_port,
                    target_port)
        end
    end
end

function pmx_utils.raise_connect_event_port_to_node(source_port, target_node,
        target_port_id)
    if source_port ~= nil and target_node ~= nil then
        local target_port = target_node:lookup_port(Interest({
            Constraint({ "port.id", "=", target_port_id }),
            Constraint({ "port.direction", "=", "in" })
        }))
        if target_port ~= nil then
            source:call("push-event", "pmx_connect_ports", source_port,
                    target_port)
        end
    end
end
return pmx_utils