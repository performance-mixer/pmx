local pmx_utils = require("pmx_utils")

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
        log:info("Added pmx-input-channels-ins")
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
        log:info("Removed pmx-input-channels-ins")
        InputChannelsIns = nil
    end,
}):register()

function connect_input_channel_outs_to_group_channels_a(input_channels_outs,
        group_channels_a_ins)
    local metadata = channel_metadata_om.lookup(
            Interest({
                type = "metadata",
                Constraint({ "metadata.name", "=", "performance-mixer" }),
            }))
    if metadata ~= nil then
        for i = 1, 32 do
            local port_meta_name = channel_group_id .. i
            local value = metadata.find(0, port_meta_name)
            if value == nil then
                value = 1
            end

            port_id = 0
            if (i - 1) % 2 == 0 then
                port_id = (value - 1) * 2
            else
                port_id = (value - 1) * 2 + 1
            end
            pmx_utils.raise_connect_event(input_channels_outs,
                    tostring(i - 1), group_channels_a_ins, port_id)
        end
    end
end

function connect_input_channel_outs_to_group_channels_b(input_channels_outs,
        group_channels_b_ins)
    local metadata = channel_metadata_om.lookup(
            Interest({
                type = "metadata",
                Constraint({ "metadata.name", "=", "performance-mixer" }),
            }))
    if metadata ~= nil then
        for i = 33, 64 do
            local port_meta_name = channel_group_id .. i
            local value = metadata.find(0, port_meta_name)
            if value == nil then
                value = 1
            end

            port_id = 0
            if (i - 1) % 2 == 0 then
                port_id = (value - 1) * 2
            else
                port_id = (value - 1) * 2 + 1
            end
            pmx_utils.raise_connect_event(input_channels_outs,
                    tostring(i - 1), group_channels_b_ins, port_id)
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
        log:info("Added pmx-input-channels-outs ...")
        InputChannelsOuts = event:get_subject()
        if InputChannelsOuts ~= nil and GroupChannelsAIns ~= nil then
            log:info("... connecting to group-channels-a-ins")
            connect_input_channel_outs_to_group_channels_a(InputChannelsOuts,
                    GroupChannelsAIns)
        else
            log:info("... but group-channels-a-ins still nil")
        end

        if InputChannelsOuts ~= nil and GroupChannelsBIns ~= nil then
            log:info("... connecting to group-channels-b-ins")
            connect_input_channel_outs_to_group_channels_b(InputChannelsOuts,
                    GroupChannelsBIns)
        else
            log:info("... but group-channels-b-ins still nil")
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
        log:info("Removed pmx-input-channels-outs ...")
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
        log:info("Added pmx-group-channels-a-ins ...")
        GroupChannelsAIns = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsAIns == nil then
            log:info("... connecting to pmx-input-channels-outs")
            connect_input_channel_outs_to_group_channels_a(InputChannelsOuts,
                    GroupChannelsAIns)
        else
            log:info("... but pmx-input-channels-outs still nil")
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
        log:info("Removed pmx-group-channels-a-ins ...")
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
        log:info("Added pmx-group-channels-b-ins ...")
        GroupChannelsBIns = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsBIns == nil then
            log:info("... connecting to pmx-input-channels-outs")
            connect_input_channel_outs_to_group_channels_b(InputChannelsOuts,
                    GroupChannelsBIns)
        else
            log:info("... but pmx-input-channels-outs still nil")
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
        log:info("Removed pmx-group-channels-b-ins ...")
        GroupChannelsBIns = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_a_outs/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-a-outs" }),
        }),
    },
    execute = function(event)
        log:info("Added pmx-group-channels-a-outs ...")
        GroupChannelsAOuts = event:get_subject()
        if not GroupChannelsAOuts == nil and not LayerChannelsIns == nil then
            log:info("... connecting to pmx-layer-channels-ins")
            pmx_utils.raise_connect_event(GroupChannelsAOuts, "0",
                    LayerChannelsIns, "0")
            pmx_utils.raise_connect_event(GroupChannelsAOuts, "1",
                    LayerChannelsIns, "1")
        else
            log:info("... but pmx-layer-channels-ins still nil")
        end
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_a_outs/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-group-channels-a-outs" }),
        }),
    },
    execute = function(_)
        log:info("Removed pmx-group-channels-a-outs ...")
        GroupChannelsAOuts = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_b_outs/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-b-outs" }),
        }),
    },
    execute = function(event)
        log:info("Added pmx-group-channels-b-outs ...")
        GroupChannelsBOuts = event:get_subject()
        if not GroupChannelsBOuts == nil and not LayerChannelsIns == nil then
            log:info("... connecting to pmx-layer-channels-ins")
            pmx_utils.raise_connect_event(GroupChannelsBOuts, "0",
                    LayerChannelsIns, "2")
            pmx_utils.raise_connect_event(GroupChannelsBOuts, "1",
                    LayerChannelsIns, "3")
        else
            log:info("... but pmx-layer-channels-ins still nil")
        end
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_b_outs/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-group-channels-b-outs" }),
        }),
    },
    execute = function(_)
        log:info("Removed pmx-group-channels-b-outs ...")
        GroupChannelsBOuts = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/layer_channels_ins/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-layer-channels-ins" }),
        }),
    },
    execute = function(event)
        log:info("Added pmx-layer-channels-ins ...")
        LayerChannelsIns = event:get_subject()
        if not GroupChannelsAOuts == nil and not LayerChannelsIns == nil then
            log:info("... connecting to pmx-group-channels-a-outs")
            pmx_utils.raise_connect_event(GroupChannelsAOuts, "0",
                    LayerChannelsIns, "0")
            pmx_utils.raise_connect_event(GroupChannelsAOuts, "1",
                    LayerChannelsIns, "1")
        else
            log:info("... but pmx-group-channels-a-outs still nil")
        end

        if not GroupChannelsBOuts == nil and not LayerChannelsIns == nil then
            log:info("... connecting to pmx-group-channels-b-outs")
            pmx_utils.raise_connect_event(GroupChannelsBOuts, "0",
                    LayerChannelsIns, "2")
            pmx_utils.raise_connect_event(GroupChannelsBOuts, "1",
                    LayerChannelsIns, "3")
        else
            log:info("... but pmx-group-channels-b-outs still nil")
        end
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/layer_channels_ins/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-layer-channels-ins" }),
        }),
    },
    execute = function(_)
        log:info("Removed pmx-layer-channels-ins ...")
        LayerChannelsIns = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/layer_channels_outs/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-layer-channels-outs" }),
        }),
    },
    execute = function(event)
        log:info("Added pmx-layer-channels-outs ...")
        LayerChannelsOuts = event:get_subject()
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/layer_channels_outs/removed",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-removed" }),
            Constraint({ "node.name", "=", "pmx-layer-channels-outs" }),
        }),
    },
    execute = function(_)
        log:info("Removed pmx-layer-channels-outs ...")
        LayerChannelsOuts = nil
    end,
}):register()
