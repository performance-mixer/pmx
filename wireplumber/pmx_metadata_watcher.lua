link_om = ObjectManager({
    Interest({ type = "link" })
})

link_om:activate()

input_channels_om = ObjectManager({
    Interest({
        type = "node",
        Constraint({ "node.name", "=", "pmx-input-channels-ins" })
    })
})

input_channels_om:activate()

port_om = ObjectManager({
    Interest({
        type = "port",
        Constraint({ "port.direction", "=", "out" }),
        Constraint({ "port.monitor", "!", "true" }),
    })
})

port_om:activate()

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
        local target_value = props["event.subject.value"]
        local input_ports_node = input_channels_om:lookup(
                Interest({
                    type = "node",
                    Constraint({ "node.name", "=", "pmx-input-channels-ins" })
                }))
        if input_ports_node ~= nil then
            if target_value ~= nil then
                local output_port = port_om:lookup(
                        Interest({
                            type = "port",
                            Constraint({ "port.alias", "=", target_value }),
                        }))
                local channel_id = string.sub(props["event.subject.key"], 14)
                local link = Link("link-factory", {
                    ["link.output.port"] = output_port.properties["object.id"],
                    ["link.input.port"] = channel_id,
                    ["link.input.node"] = input_ports_node.properties["object.id"],
                    ["object.linger"] = true
                })
                link:activate(1)
            else
                for link in link_om:iterate(Interest({
                    type = "link",
                    Constraint({ "link.input.port", "=", channel_id }),
                    Constraint({ "link.input.node", "=", input_ports_node.properties["object.id"] })
                })) do
                    link:request_destroy()
                end
            end
        end
    end,
}):register()
