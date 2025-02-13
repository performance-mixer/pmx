GroupChannelsAOuts = nil
GroupChannelsBOuts = nil
LayerChannelsIns = nil

Links = {}

function connect_group_a_to_layer(source)
    local success, err = pcall(function()
        print("... connecting group a to pmx-layer-channels-ins")
        print("Connecting " ..
                GroupChannelsAOuts.properties["object.id"] .. ", " .. 0 .. " => " ..
                LayerChannelsIns.properties["object.id"] .. ", " .. 0)
        local link = Link("link-factory", {
            ["link.output.port"] = 0,
            ["link.input.port"] = 0,
            ["link.output.node"] = tonumber(GroupChannelsAOuts.properties["object.id"]),
            ["link.input.node"] = tonumber(LayerChannelsIns.properties["object.id"]),
            ["object.linger"] = true
        })
        link:activate(1)
        table.insert(Links, link)

        print("Connecting " ..
                GroupChannelsAOuts.properties["object.id"] .. ", " .. 1 .. " => " ..
                LayerChannelsIns.properties["object.id"] .. ", " .. 1)
        link = Link("link-factory", {
            ["link.output.port"] = 1,
            ["link.input.port"] = 1,
            ["link.output.node"] = tonumber(GroupChannelsAOuts.properties["object.id"]),
            ["link.input.node"] = tonumber(LayerChannelsIns.properties["object.id"]),
            ["object.linger"] = true
        })
        link:activate(1)
        table.insert(Links, link)
    end)

    if not success then
        print("Error while calling connect_group_a_to_layer: ", err)
        return err
    else
        print("... connected group a to pmx-layer-channels-ins")
    end
end

function connect_group_b_to_layer(source)
    local success, err = pcall(function()
        print("... connecting group b to pmx-layer-channels-ins")

        print("Connecting " ..
                GroupChannelsBOuts.properties["object.id"] .. ", " .. 0 .. " => " ..
                LayerChannelsIns.properties["object.id"] .. ", " .. 2)
        local link = Link("link-factory", {
            ["link.output.port"] = 0,
            ["link.input.port"] = 2,
            ["link.output.node"] = GroupChannelsBOuts.properties["object.id"],
            ["link.input.node"] = LayerChannelsIns.properties["object.id"],
            ["object.linger"] = true
        })
        link:activate(1)
        table.insert(Links, link)

        print("Connecting " ..
                GroupChannelsBOuts.properties["object.id"] .. ", " .. 1 .. " => " ..
                LayerChannelsIns.properties["object.id"] .. ", " .. 3)
        link = Link("link-factory", {
            ["link.output.port"] = 1,
            ["link.input.port"] = 3,
            ["link.output.node"] = GroupChannelsBOuts.properties["object.id"],
            ["link.input.node"] = LayerChannelsIns.properties["object.id"],
            ["object.linger"] = true
        })
        link:activate(1)
        table.insert(Links, link)
    end)

    if not success then
        print("Error while calling connect_group_b_to_layer: ", err)
        return err
    else
        print("... connected group b to pmx-layer-channels-ins")
    end
end

SimpleEventHook({
    name = "pmx/link_watcher/group_channels_a_outs/added",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-group-channels-a-outs" }),
        }),
    },
    execute = function(event)
        print("Added pmx-group-channels-a-outs ...")
        GroupChannelsAOuts = event:get_subject()
        if LayerChannelsIns ~= nil then
            local source = event:get_source()
            connect_group_a_to_layer(source)
        else
            print("... but pmx-layer-channels-ins still nil")
        end
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
        print("Added pmx-group-channels-b-outs ...")
        GroupChannelsBOuts = event:get_subject()
        if LayerChannelsIns ~= nil then
            local source = event:get_source()
            connect_group_b_to_layer(source)
        else
            print("... but pmx-layer-channels-ins still nil")
        end
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
        print("Added pmx-layer-channels-ins ...")
        LayerChannelsIns = event:get_subject()
        local source = event:get_source()
        if GroupChannelsAOuts ~= nil then
            connect_group_a_to_layer(source)
        else
            print("... but pmx-group-channels-a-outs still nil")
        end

        if GroupChannelsBOuts ~= nil then
            connect_group_b_to_layer(source)
        else
            print("... but pmx-group-channels-b-outs still nil")
        end
    end,
}):register()
