InputChannelsIns = nil
InputChannelsOuts = nil

GroupChannelsAIns = nil
GroupChannelsAOuts = nil

GroupChannelsBIns = nil
GroupChannelsBOuts = nil

LayerChannelsIns = nil
LayerChannelsOuts = nil

local log = Log.open_topic("s-filter-watcher")

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
    local metadata = channel_metadata_om:lookup(
            Interest({
                type = "metadata",
                Constraint({ "metadata.name", "=", "performance-mixer" }),
            }))
    if metadata ~= nil then
        for i = 1, 32 do
            local port_meta_name = "channel_group_id_" .. i
            local value = metadata:find(0, port_meta_name)
            if value == nil then
                value = 1
            end

            port_id = 0
            if (i - 1) % 2 == 0 then
                port_id = (value - 1) * 2
            else
                port_id = (value - 1) * 2 + 1
            end
            local props = {
                ["source_port_id"] = tostring(i - 1),
                ["source_node_id"] = InputChannelsOuts.properties["object.id"],
                ["target_port_id"] = port_id,
                ["target_node_id"] = GroupChannelsAIns.properties["object.id"],
            }
            source:call("push-event", "pmx_connect_ports", nil,
                    props)
        end
    end
end

function connect_input_channel_outs_to_group_channels_b(source)
    local metadata = channel_metadata_om:lookup(
            Interest({
                type = "metadata",
                Constraint({ "metadata.name", "=", "performance-mixer" }),
            }))
    if metadata ~= nil then
        for i = 33, 64 do
            local port_meta_name = "channel_group_id_" .. i
            local value = metadata:find(0, port_meta_name)
            if value == nil then
                value = 1
            end

            port_id = 0
            if (i - 1) % 2 == 0 then
                port_id = (value - 1) * 2
            else
                port_id = (value - 1) * 2 + 1
            end
            local props = {
                ["source_port_id"] = tostring(i - 1),
                ["source_node_id"] = InputChannelsOuts.properties["object.id"],
                ["target_port_id"] = port_id,
                ["target_node_id"] = GroupChannelsBIns.properties["object.id"],
            }
            source:call("push-event", "pmx_connect_ports", nil,
                    props)
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
            -- connect_input_channel_outs_to_group_channels_a(source)
        else
            print("... but group-channels-a-ins still nil")
        end

        if InputChannelsOuts ~= nil and GroupChannelsBIns ~= nil then
            print("... connecting to group-channels-b-ins")
            -- connect_input_channel_outs_to_group_channels_b(source)
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
            -- connect_input_channel_outs_to_group_channels_a(source)
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
            -- connect_input_channel_outs_to_group_channels_b(source)
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
