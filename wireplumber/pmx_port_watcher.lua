local pmx_utils = require("pmx_utils")

channel_metadata_om = ObjectManager({
    Interest({
        type = "metadata",
        Constraint({ "metadata.name", "=", "performance-mixer" }),
    }),
})

channel_metadata_om:activate()

input_channels_om = ObjectManager({
    Interest({
        type = "node",
        Constraint({ "node.name", "=", "pmx-input-channels-ins" })
    })
})

input_channels_om:activate()

SimpleEventHook({
    name = "pmx/port_watcher/capture_ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.monitor", "!", "true" }),
        }),
    },
    execute = function(event)
        for metadata in channel_metadata_om:iterate() do
            for channel_id = 0, 31 do
                local channel_key = "channel_port_" .. channel_id
                local value = metadata:find(0, channel_key)

                if value ~= nil then
                    local port = event:get_subject()
                    if port.properties["port.alias"] == value then
                        local input_ports_node = input_channels_om:lookup(
                                Interest({
                                    type = "node",
                                    Constraint({ "node.name", "=", "pmx-input-channels-ins" })
                                }))
                        pmx_utils.raise_connect_event(port, input_ports_node,
                                channel_id)
                    end
                end
            end
        end
    end,
}):register()
