InputChannelsIns = nil
InputChannelsOuts = nil

GroupChannelsAIns = nil
GroupChannelsAOuts = nil

GroupChannelsBIns = nil
GroupChannelsBOuts = nil

LayerChannelsIns = nil
LayerChannelsOuts = nil

local log = Log.open_topic("s-filter-watcher")

local link_om = ObjectManager({
    Interest({ type = "link" })
})

link_om:activate()

local channel_metadata_om = ObjectManager({ Interest({
    type = "metadata",
    Constraint({ "metadata.name", "=", "performance-mixer" }),
}) })

channel_metadata_om:activate()

SimpleEventHook({
    name = "pmx/link_watcher/input_channels_ins/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-input-channels-ins" }),
        }),
    },
    execute = function(event)
        print("Added pmx-input-channels-ins")
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/input_channels_ins/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-input-channels-ins" }),
        }),
    },
    execute = function(_)
        print("Removed pmx-input-channels-ins")
        InputChannelsIns = nil
    end,
}):register()

function connect_input_channel_outs_to_group_channels_a(source)
    print("connecting input channels to group a")
    local metadata = channel_metadata_om:lookup(
            Interest({
                type = "metadata",
                Constraint({ "metadata.name", "=", "performance-mixer" }),
            }))
    if metadata ~= nil then
        for input_ports_id = 0, 31 do
            local port_meta_name = "channel_group_id_" .. input_ports_id
            print("... checking metadata " .. port_meta_name)
            local value = metadata:find(0, port_meta_name)
            if value == nil then
                print("... metadata not found, using group 1")
                value = 0
            end

            port_id = 0
            if input_ports_id % 2 == 0 then
                port_id = value * 2
            else
                port_id = value * 2 + 1
            end
            print("... connecting to port id " .. port_id)

            local link = Link("link-factory", {
                ["link.output.port"] = input_ports_id,
                ["link.input.port"] = port_id,
                ["link.output.node"] = InputChannelsOuts.properties["object.id"],
                ["link.input.node"] = GroupChannelsAIns.properties["object.id"],
                ["object.linger"] = true
            })
            link:activate(1)
        end
    else
        print("... couldn't find metadata performance-mixer")
    end
end

function connect_input_channel_outs_to_group_channels_b(source)
    print("connecting input channels to group b")
    local metadata = channel_metadata_om:lookup(
            Interest({
                type = "metadata",
                Constraint({ "metadata.name", "=", "performance-mixer" }),
            }))
    if metadata ~= nil then
        local input_node_id = GroupChannelsBIns.properties["object.id"]
        for input_ports_id = 32, 63 do
            local port_meta_name = "channel_group_id_" .. input_ports_id
            print("... checking metadata " .. port_meta_name)
            local value = metadata:find(0, port_meta_name)
            if value == nil then
                print("... metadata not found, using group 1")
                value = 0
            end

            port_id = 0
            if input_ports_id % 2 == 0 then
                port_id = value * 2
            else
                port_id = value * 2 + 1
            end
            print("... connecting to port id " .. port_id)

            local output_port = InputChannelsOuts:lookup_port({
                Constraint({ "port.id", "=", input_ports_id }),
                Constraint({ "port.direction", "=", "out" }),
            })

            local link = Link("link-factory", {
                ["link.output.port"] = output_port.properties["object.id"],
                ["link.input.port"] = port_id,
                ["link.input.node"] = input_node_id,
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
end

SimpleEventHook({
    name = "pmx/link_watcher/input_channels_outs/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-input-channels-outs" }),
        }),
    },
    execute = function(event)
        local source = event:get_source()
        print("Added pmx-input-channels-outs ...")
        InputChannelsOuts = event:get_subject()
        if InputChannelsOuts ~= nil and GroupChannelsAIns ~= nil then
            print("... connecting to group-channels-a-ins")
            connect_input_channel_outs_to_group_channels_a(source)
        else
            print("... but group-channels-a-ins still nil")
        end

        if InputChannelsOuts ~= nil and GroupChannelsBIns ~= nil then
            print("... connecting to group-channels-b-ins")
            connect_input_channel_outs_to_group_channels_b(source)
        else
            print("... but group-channels-b-ins still nil")
        end
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/input_channels_outs/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-input-channels-outs" }),
        }),
    },
    execute = function(_)
        print("Removed pmx-input-channels-outs ...")
        InputChannelsOuts = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_a_ins/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-a-ins" }),
        }),
    },
    execute = function(event)
        print("Added pmx-group-channels-a-ins ...")
        local source = event:get_source()
        GroupChannelsAIns = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsAIns == nil then
            print("... connecting to pmx-input-channels-outs")
            connect_input_channel_outs_to_group_channels_a(source)
        else
            print("... but pmx-input-channels-outs still nil")
        end
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_a_ins/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-group-channels-a-ins" }),
        }),
    },
    execute = function(_)
        print("Removed pmx-group-channels-a-ins ...")
        GroupChannelsAIns = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_b_ins/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-b-ins" }),
        }),
    },
    execute = function(event)
        print("Added pmx-group-channels-b-ins ...")
        local source = event:get_source()
        GroupChannelsBIns = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsBIns == nil then
            print("... connecting to pmx-input-channels-outs")
            connect_input_channel_outs_to_group_channels_b(source)
        else
            print("... but pmx-input-channels-outs still nil")
        end
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_b_ins/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-group-channels-b-ins" }),
        }),
    },
    execute = function(_)
        print("Removed pmx-group-channels-b-ins ...")
        GroupChannelsBIns = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/metadata_watcher/watch/channel_port",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "metadata-changed" }),
            Constraint({ "metadata.name", "=", "performance-mixer" }),
            Constraint({ "event.subject.key", "matches", "channel_group_id_*" }),
        }),
    },
    execute = function(event)
        local channel_id = string.sub(props["event.subject.key"], 18)
        local props = event:get_properties()
        local target_value = props["event.subject.value"]

        local source_node = InputChannelsOuts
        local target_node = nil
        if channel_id >= 32 then
            target_node = GroupChannelsBIns
        else
            target_node = GroupChannelsAIns
        end

        local source_port = source_node:lookup_port({
            Constraint({ "port.id", "=", channel_id }),
            Constraint({ "port.direction", "=", "out" }),
        })

        local group_port_id = 0
        if channel_id % 2 == 0 then
            group_port_id = target_value * 2
        else
            group_port_id = target_value * 2 + 1
        end
        local target_port = target_port:lookup_port({
            Constraint({ "port.id", "=", }),
            Constraint({ "port.direction", "=", "out" }),
        })

        local old_link = link_om.lookup(Interest({
            type = "link",
            Constraint({ "link.output.port", "=", source_port.properties["object.id"] })
        }))
        if old_link ~= nil and old_link.properties["link.input.port"] ~= target_port.properties["object.id"] then
            old_link:request_destroy()
        elseif old_link == nil then
            local link = Link("link-factory", {
                ["link.output.port"] = source_port.properties["object.id"],
                ["link.input.port"] = target_port.properties["object.id"],
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end,
}):register()
