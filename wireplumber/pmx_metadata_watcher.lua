link_om = ObjectManager({
    Interest({ type = "link" })
}):activate()

input_channels_om = ObjectManager({
    Interest({
        type = "node",
        Constraint({ "node.name", "=", "pmx-input-channels-ins" })
    })
}):activate()

port_om = ObjectManager({
    Interest({
        type = "port",
        Constraint({ "port.direction", "=", "out" }),
        Constraint({ "port.monitor", "!", "true" }),
    })
}):activate()

SimpleEventHook({
    name = "pmx/metadata_watcher/watch/channel_port",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "metadata-changed" }),
            Constraint({ "metadata.name", "=", "performance-mixer" }),
            Constraint({ "event.subject.key", "matches", "channel_port_*" }),
        }),
    },
    execute = function(event)
        local props = event:get_properties()
        local target_key = props["event.subject.key"]
        local target_value = props["event.subject.value"]
        local target_port_id = string.sub(target_key, 14)
        local input_ports_node = input_channels_om:lookup(
                Interest({
                    type = "node",
                    Constraint({ "node.name", "=", "pmx-input-channels-ins" })
                }))
        if input_ports_node ~= nil then
            local input_port = input_ports_node:lookup_port(
                    Interest({ type = "port",
                               Constraint({ "port.id", "=", channel_id }),
                    }))

            if input_port ~= nil then
                if target_value ~= nil then
                    local output_port = port_om.lookup(
                            Interest({ type = "port",
                                       Constraint({ "port.alias", "=", target_value }),
                            }))
                    if output_port ~= nil then
                        local source = event:get_source()
                        source:call("push-event",
                                "connect/capture_port/input_port",
                                output_port,
                                input_port)
                    end
                else
                    for link in link_om:iterate(Interest({
                        type = "link",
                        Constraint({ "link.input.port", "=", channel_id }),
                        Constraint({ "link.input.node", "=", input_ports_node.properties["object.id"] })
                    })) do
                        local source = event:get_source()
                        source:call("push-event",
                                "delete/link",
                                link,
                                nil)
                        link:request_destroy()
                    end
                end
            end
        end
    end,
}):register()
