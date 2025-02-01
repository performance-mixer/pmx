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

link_om = ObjectManager({
    Interest({ type = "link" })
})

link_om:activate()

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
        local source = event:get_source()
        source:call("push-event", "connect_filter_inputs", nil, nil)
    end,
}):register()

SimpleEventHook({
    name = "pmx/connect_filter_inputs",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect_filter_inputs" }),
        }),
    },
    execute = function(event)
        local filter_node = filter_om:lookup(Interest({
            type = "node",
            Constraint({ "node.name", "=", "pmx input channels - ins" })
        }))
        if filter_node ~= nil then
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
                        if source_port ~= nil and filter_node ~= nil then
                            MyLink = Link("link-factory", {
                                ["link.output.port"] = source_port.properties["port.id"],
                                ["link.input.port"] = channel_id,
                                ["link.output.node"] = source_port.properties["node.id"],
                                ["link.input.node"] = filter_node.properties["object.id"],
                            })
                            MyLink:activate(1)
                        end
                    else
                        print('Deleting links', channel_id, filter_node.properties["object.id"])
                        for link in link_om:iterate(Interest({
                            type = "link",
                            Constraint({ "link.input.port", "=", channel_id }),
                            Constraint({ "link.input.node", "=", filter_node.properties["object.id"] })
                        })) do
                            link:request_destroy()
                        end
                    end
                end
            end
        else
            print("Filter not found")
        end
    end,
}):register()
