local pmx_utils = require("pmx_utils")

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
                local output_port = port_om.lookup(
                        Interest({ type = "port",
                                   Constraint({ "port.alias", "=", target_value }),
                        }))
                pmx_utils.raise_connect_event_port_to_node(output_port,
                        input_ports_node, channel_id)
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
    end,
}):register()
