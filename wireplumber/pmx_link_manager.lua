SimpleEventHook({
    name = "pmx/link_manager/connect/input_ports/input_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect/input_ports/input_channels" }),
        }),
    },
    execute = function(event)
        print('connecting ports to input channels')
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_manager/connect/input_channels/group_channels_a",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect/input_channels/group_channels_a" }),
        }),
    },
    execute = function(event)
        print('connecting input channels to group channels a')
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_manager/connect/input_channels/group_channels_b",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect/input_channels/group_channels_b" }),
        }),
    },
    execute = function(event)
        print('connecting input channels to group channels b')
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_manager/connect/group_channels_a/layer_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect/group_channels_a/layer_channels" }),
        }),
    },
    execute = function(event)
        print('connecting group channels b to layer channels')
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_manager/connect/group_channels_b/layer_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect/group_channels_b/layer_channels" }),
        }),
    },
    execute = function(event)
        print('connecting group channels b to layer channels')
    end,
}):register()

SimpleEventHook({
    name = "pmx/link_manager/connect/capture_port/input_port",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "connect/capture_port/input_port" }),
        }),
    },
    execute = function(event)
        print('connecting capture port to input port')
    end,
}):register()
