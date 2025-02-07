InputChannelsIns = nil
InputChannelsOuts = nil

GroupChannelsAIns = nil
GroupChannelsAOuts = nil

GroupChannelsBIns = nil
GroupChannelsBOuts = nil

LayerChannelsIns = nil
LayerChannelsOuts = nil

SimpleEventHook({
    name = "pmx/link_watcher/input_channels_ins/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-input-channels-ins" }),
        }),
    },
    execute = function(event)
        InputChannelsIns = event:get_subject()
        local source = event:get_source()
        source:call("push-event",
                "connect/input_ports/input_channels", InputChannelsIns,
                nil)
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
        InputChannelsIns = nil
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_watcher/input_channels_outs/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-input-channels-outs" }),
        }),
    },
    execute = function(event)
        InputChannelsOuts = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsAIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/input_channels/group_channels_a",
                    InputChannelsOuts,
                    GroupChannelsAIns)
        end
        if not InputChannelsOuts == nil and not GroupChannelsBIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/input_channels/group_channels_a",
                    InputChannelsOuts,
                    GroupChannelsBIns)
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
        GroupChannelsAIns = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsAIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/input_channels/group_channels_a",
                    InputChannelsOuts,
                    GroupChannelsAIns)
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
        GroupChannelsBIns = event:get_subject()
        if not InputChannelsOuts == nil and not GroupChannelsBIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/input_channels/group_channels_b",
                    InputChannelsOuts,
                    GroupChannelsBIns)
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
        GroupChannelsAOuts = event:get_subject()
        if not GroupChannelsAOuts == nil and not LayerChannelsIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/group_channels_a/layer_channels",
                    GroupChannelsAOuts,
                    LayerChannelsIns)
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
        GroupChannelsBOuts = event:get_subject()
        if not GroupChannelsBOuts == nil and not LayerChannelsIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/group_channels_b/layer_channels",
                    GroupChannelsBOuts,
                    LayerChannelsIns)
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
        LayerChannelsIns = event:get_subject()
        if not GroupChannelsAOuts == nil and not LayerChannelsIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/group_channels_a/layer_channels",
                    GroupChannelsAOuts,
                    LayerChannelsIns)
        end
        if not GroupChannelsBOuts == nil and not LayerChannelsIns == nil then
            local source = event:get_source()
            source:call("push-event",
                    "connect/group_channels_b/layer_channels",
                    GroupChannelsBOuts,
                    LayerChannelsIns)
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
        LayerChannelsOuts = nil
    end,
}):register()
