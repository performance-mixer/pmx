channel_metadata_om = ObjectManager({
    Interest({
        type = "metadata",
        Constraint({ "metadata.name", "=", "performance-mixer" }),
    }),
})

channel_metadata_om:activate()

filter_om = ObjectManager({
    Interest({
        type = "node",
        Constraint({ "node.name", "=", "pmx input channels - ins" })
    })
})

filter_om:activate()

output_ports_om = ObjectManager({
    Interest({
        type = "port",
        Constraint({ "port.direction", "=", "out" }),
    })
})

output_ports_om:activate()

SimpleEventHook({
    name = "pmx/process_metadata",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "metadata-changed" }),
            Constraint({ "metadata.name", "=", "performance-mixer" }),
            Constraint({ "event.subject.key", "matches", "channel_*" }),
        }),
    },
    execute = function(event)
        print("Metadata changed")
        local source = event:get_source()
        source:call("push-event", "pmx_create_filters", nil, nil)
    end,
}):register()

SimpleEventHook({
    name = "pmx/create_filters",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "pmx_create_filters" }),
        }),
    },
    execute = function(event)
        for metadata in channel_metadata_om:iterate() do
            for channel_id = 0, 31 do
                local channel_key = "channel_" .. channel_id
                local value = metadata:find(0, channel_key)

                if value ~= nil then
                    local source_port = output_ports_om:lookup(Interest({
                        type = "port",
                        Constraint({ "port.direction", "=", "out" }),
                        Constraint({ "port.alias", "=", value })
                    }))
                    local filter_node = filter_om:lookup(Interest({
                        type = "node",
                        Constraint({ "node.name", "=", "pmx input channels - ins" })
                    }))
                    if source_port ~= nil and filter_node ~= nil then
                        MyLink = Link("link-factory", {
                            ["link.output.port"] = source_port.properties["port.id"],
                            ["link.input.port"] = channel_id,
                            ["link.output.node"] = source_port.properties["node.id"],
                            ["link.input.node"] = filter_node.properties["object.id"],
                        })
                        MyLink:activate(1)
                    end
                end
            end
        end
    end,
}):register()
