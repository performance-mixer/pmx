local filter_chain
SimpleEventHook({
    name = "set_up_filter_chain/find_filter_chain",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-filter-chain-ctrl" }),
        }),
    },
    execute = function(event)
        print("found filter chain")
        filter_chain = event:get_subject()
        local source = event:get_source()
        source:call("push-event", "update-fc-ids", nil, nil)
    end
}):register()

local layer_channels
SimpleEventHook({
    name = "set_up_filter_chain/find_layer_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-layer-channels-ins" }),
        })
    },
    execute = function(event)
        print("found layer channels")
        layer_channels = event:get_subject()
        local source = event:get_source()
        source:call("push-event", "update-fc-ids", nil, nil)
    end
}):register()

local input_channels
SimpleEventHook({
    name = "set_up_filter_chain/find_input_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-input-channels-ins" }),
        })
    },
    execute = function(event)
        print("found input channels")
        input_channels = event:get_subject()
        local source = event:get_source()
        source:call("push-event", "update-fc-ids", nil, nil)
    end
}):register()

local group_a_channels
SimpleEventHook({
    name = "set_up_filter_chain/find_group_a_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-a-ins" }),
        })
    },
    execute = function(event)
        print("found group a channels")
        group_a_channels = event:get_subject()
        local source = event:get_source()
        source:call("push-event", "update-fc-ids", nil, nil)
    end
}):register()

local group_b_channels
SimpleEventHook({
    name = "set_up_filter_chain/find_group_b_channels",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-b-ins" }),
        })
    },
    execute = function(event)
        print("found group b channels")
        group_b_channels = event:get_subject()
        local source = event:get_source()
        source:call("push-event", "update-fc-ids", nil, nil)
    end
}):register()

SimpleEventHook({
    name = "set_up_filter_chain/set_filter_chain_parameters",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "update-fc-ids" }),
        })
    },
    execute = function(event)
        if filter_chain ~= nil then
            local input_channels_id
            if input_channels ~= nil then
                input_channels_id = tonumber(input_channels.properties["object.id"])
            end

            local group_a_channels_id
            if group_a_channels ~= nil then
                group_a_channels_id = tonumber(group_a_channels.properties["object.id"])
            end

            local group_b_channels_id
            if group_b_channels ~= nil then
                group_b_channels_id = tonumber(group_b_channels.properties["object.id"])
            end

            local layer_channels_id
            if layer_channels ~= nil then
                layer_channels_id = tonumber(layer_channels.properties["object.id"])
            end

            print("inputChannels", input_channels_id)
            print("groupChannelsA", group_a_channels_id)
            print("groupChannelsB", group_b_channels_id)
            print("layerChannels", layer_channels_id)

            local props = {
                "Spa:Pod:Object:Param:Props",
                "Props",
                params = Pod.Struct {
                    "inputChannels", input_channels_id,
                    "groupChannelsA", group_a_channels_id,
                    "groupChannelsB", group_b_channels_id,
                    "layerChannels", layer_channels_id
                }
            }

            local param = Pod.Object(props)
            filter_chain:set_param("Props", param)
        end
    end
}):register()
