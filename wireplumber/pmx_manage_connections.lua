MyObjectManager = ObjectManager({
    Interest({ type = "port" }),
    Interest({ type = "node" }),
    Interest({ type = "metadata" }),
    Interest({ type = "link" }),
})

MyObjectManager:activate()

-- This handles the initial connections to the input module of the filter chain,
-- when the filter is first created
SimpleEventHook({
    name = "connect_initial_fc_inputs",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "in" }),
            Constraint({ "port.alias", "matches", "PMX Input Channels:input*" })
        })
    },
    execute = function(event)
        local target_port = event:get_subject()

        local name = "channel_port_" .. (target_port.properties["port.id"] + 1)
        print("looking up metadata for name", name)
        local metadata = MyObjectManager:lookup(
                Interest({
                    type = "metadata",
                    Constraint({ "metadata.name", "=", "performance-mixer" }),
                }))
        if metadata ~= nil then
            local port_name_meta = metadata:find(0, name)
            if port_name_meta ~= nil then
                print("Connecting to port with alias", port_name_meta)
                local source_port = MyObjectManager:lookup(
                        Interest({
                            type = "port",
                            Constraint({ "port.direction", "=", "out" }),
                            Constraint({ "port.alias", "=", port_name_meta }),
                        }))
                if source_port ~= nil then
                    print("... creating new link")
                    local link = Link("link-factory", {
                        ["link.output.port"] = source_port.properties["object.id"],
                        ["link.input.port"] = target_port.properties["object.id"],
                        ["object.linger"] = true
                    })
                    link:activate(1)
                end
            end
        end
    end
}):register()

-- this handles all out ports that are added after the filter chain was created
-- we have to look if we can find the port in the values for the metadata and
-- act on it if we find it there
SimpleEventHook({
    name = "connect_initial_ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
        })
    },
    execute = function(event)
        local source_port = event:get_subject()

        print("evaluating port", source_port.properties["port.alias"])

        local metadata = MyObjectManager:lookup(
                Interest({
                    type = "metadata",
                    Constraint({ "metadata.name", "=", "performance-mixer" }),
                }))

        if metadata ~= nil then
            for _, key, _, value in metadata:iterate(Id.ANY) do
                if value == source_port.properties["port.alias"] then
                    local port_id = tonumber(string.sub(key, 14))
                    local target_port_alias = "PMX Input Channels:input_" .. port_id;

                    local target_port = MyObjectManager:lookup(
                            Interest({
                                type = "port",
                                Constraint({ "port.direction", "=", "in" }),
                                Constraint({ "port.alias", "=", target_port_alias }),
                            }))
                    if target_port ~= nil then
                        print("... creating new link")
                        local link = Link("link-factory", {
                            ["link.output.port"] = source_port.properties["object.id"],
                            ["link.input.port"] = target_port.properties["object.id"],
                            ["object.linger"] = true
                        })
                        link:activate(1)
                    end
                end
            end
        end
    end
}):register()

-- Creates the links from sooperlooper ports to group a ports
SimpleEventHook({
    name = "connect_looper_to_group_a",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.alias", "matches", "PMX Group Channels A:input_*" })
        })
    },
    execute = function(event)
        local target_port = event:get_subject()
        local input_id = target_port.properties["port.id"] + 1

        local looper_out_ports = {
            "sooperlooper:loop0_out_1",
            "sooperlooper:loop0_out_2",
            "sooperlooper:loop1_out_1",
            "sooperlooper:loop1_out_2",
            "sooperlooper:loop2_out_1",
            "sooperlooper:loop2_out_2",
            "sooperlooper:loop3_out_1",
            "sooperlooper:loop3_out_2",
        }

        source_port_alias = looper_out_ports[input_id]

        local source_port = MyObjectManager:lookup(
                Interest({
                    type = "port",
                    Constraint({ "port.direction", "=", "out" }),
                    Constraint({ "port.alias", "=", source_port_alias }),
                }))
        if source_port ~= nil then
            print("... creating new link")
            local link = Link("link-factory", {
                ["link.output.port"] = source_port.properties["object.id"],
                ["link.input.port"] = target_port.properties["object.id"],
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

-- Creates the links from sooperlooper ports to group b ports
SimpleEventHook({
    name = "connect_looper_to_group_b",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.alias", "matches", "PMX Group Channels B:input_*" })
        })
    },
    execute = function(event)
        local target_port = event:get_subject()
        local input_id = target_port.properties["port.id"] + 1

        local looper_out_ports = {
            "sooperlooper:loop4_out_1",
            "sooperlooper:loop4_out_2",
            "sooperlooper:loop5_out_1",
            "sooperlooper:loop5_out_2",
            "sooperlooper:loop6_out_1",
            "sooperlooper:loop6_out_2",
            "sooperlooper:loop7_out_1",
            "sooperlooper:loop7_out_2",
        }

        source_port_alias = looper_out_ports[input_id]

        local source_port = MyObjectManager:lookup(
                Interest({
                    type = "port",
                    Constraint({ "port.direction", "=", "out" }),
                    Constraint({ "port.alias", "=", source_port_alias }),
                }))
        if source_port ~= nil then
            print("... creating new link")
            local link = Link("link-factory", {
                ["link.output.port"] = source_port.properties["object.id"],
                ["link.input.port"] = target_port.properties["object.id"],
                ["object.linger"] = true
            })
            link:activate(1)
        end
    end
}):register()

-- As the input channels are created after sooperlooper, we'll create the
-- initial connections to the group sooperloopers here
SimpleEventHook({
    name = "connect_initial_group_inputs",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.alias", "matches", "PMX Input Channels:output*" })
        })
    },
    execute = function(event)
        local source_port = event:get_subject()
        local input_id = source_port.properties["port.id"] + 1
        local group_a = true
        local meta_key = "channel_group_" .. input_id
        if input_id > 32 then
            group_a = false
            meta_key = "channel_group_" .. (input_id - 32)
        end

        print("looking up metadata for key", meta_key)
        local metadata = MyObjectManager:lookup(
                Interest({
                    type = "metadata",
                    Constraint({ "metadata.name", "=", "performance-mixer" }),
                }))
        if metadata ~= nil then
            local target_group = metadata:find(0, meta_key)

            local target_port = nil
            local target_port_id = (tonumber(target_group) - 1) * 2 + (1 - (input_id % 2)) + 1
            if group_a == true then
                local target_port_alias = "sooperlooper:loop" .. math.floor(target_port_id / 2) - 1 .. "_in_" .. target_port_id % 2
                print("looking for", target_port_alias)

                target_port = MyObjectManager:lookup(
                        Interest({
                            type = "port",
                            Constraint({ "port.direction", "=", "in" }),
                            Constraint({ "port.alias", "=", target_port_alias }),
                        }))
                if target_port ~= nil then
                    print("... creating new link")
                    local link = Link("link-factory", {
                        ["link.output.port"] = source_port.properties["object.id"],
                        ["link.input.port"] = target_port.properties["object.id"],
                        ["object.linger"] = true
                    })
                    link:activate(1)
                end
            else
                local target_port_alias = "sooperlooper:loop" .. math.floor(target_port_id / 2) + 3 .. "_in_" .. target_port_id % 2
                print("looking for", target_port_alias)

                target_port = MyObjectManager:lookup(
                        Interest({
                            type = "port",
                            Constraint({ "port.direction", "=", "in" }),
                            Constraint({ "port.alias", "=", target_port_alias }),
                        }))
                if target_port ~= nil then
                    print("... creating new link")
                    local link = Link("link-factory", {
                        ["link.output.port"] = source_port.properties["object.id"],
                        ["link.input.port"] = target_port.properties["object.id"],
                        ["object.linger"] = true
                    })
                    link:activate(1)
                end
            end
        end
    end
}):register()

-- We need to disconnect all links from the input channel and then create the
-- new links to the ports when the metadata for a port to channel mapping
-- changes
SimpleEventHook({
    name = "watch_channel_port_mapping_metadata",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "metadata-changed" }),
            Constraint({ "metadata.name", "=", "performance-mixer" }),
            Constraint({ "event.subject.key", "matches", "channel_port_*" }),
        }),
    },
    execute = function(event)
        print("Channel port metadata changed");
        local properties = event:get_properties()
        local key = properties["event.subject.key"]
        print("Processing key", key)

        local input_id = string.sub(key, 14)
        local port_alias = "PMX Input Channels:input_" .. input_id;
        print("Looking up port alias", port_alias)

        local port = MyObjectManager:lookup(
                Interest({
                    type = "port",
                    Constraint({ "port.alias", "=", port_alias }),
                }))

        if port ~= nil then
            local object_id = tonumber(port.properties["object.id"])
            print("Looking up links with input port", object_id)
            for link in MyObjectManager:iterate(
                    Interest({
                        type = "link",
                        Constraint({ "link.input.port", "=", object_id }),
                    })) do
                print("destroying existing link")
                link:request_destroy()
            end

            local new_value = properties["event.subject.value"]
            print("looking for port with alias", new_value)
            local source_port = MyObjectManager:lookup(
                    Interest({
                        type = "port",
                        Constraint({ "port.direction", "=", "out" }),
                        Constraint({ "port.alias", "=", new_value }),
                    }))

            if source_port ~= nil then
                print("... creating new link")
                local link = Link("link-factory", {
                    ["link.output.port"] = source_port.properties["object.id"],
                    ["link.input.port"] = port.properties["object.id"],
                    ["object.linger"] = true
                })
                link:activate(1)
            end
        end
    end
}):register()

DeletedPorts = {}

function update_group_assignment(source_input_id, group_id, group_a)
    DeletedPorts = {}
    local port_alias = "PMX Input Channels:output_" .. source_input_id;
    print("Evaluating source alias", port_alias)

    local source_port = MyObjectManager:lookup(
            Interest({
                type = "port",
                Constraint({ "port.alias", "=", port_alias }),
            }))

    if source_port ~= nil then
        local source_port_id = tonumber(source_port.properties["object.id"])
        print("Found source port, removing existing links for port",
                source_port_id)
        for link in MyObjectManager:iterate(
                Interest({
                    type = "link",
                    Constraint({ "link.output.port", "=", source_port_id }),
                })) do
            print("destroying existing link")
            link:request_destroy()
        end

        local target_port_id = (tonumber(group_id) - 1) * 2 + (1 - (source_input_id % 2)) + 1
        if group_a == true then
            local target_port_alias = "sooperlooper:loop" .. math.floor(target_port_id / 2) - 1 .. "_in_" .. target_port_id % 2
            print("looking for", target_port_alias)

            local target_port = MyObjectManager:lookup(
                    Interest({
                        type = "port",
                        Constraint({ "port.direction", "=", "in" }),
                        Constraint({ "port.alias", "=", target_port_alias }),
                    }))
            if target_port ~= nil then
                print("... creating new link")
                local link = Link("link-factory", {
                    ["link.output.port"] = source_port.properties["object.id"],
                    ["link.input.port"] = target_port.properties["object.id"],
                    ["object.linger"] = true
                })
                link:activate(1)
            end
        else
            local target_port_alias = "sooperlooper:loop" .. math.floor(target_port_id / 2) + 3 .. "_in_" .. target_port_id % 2
            print("looking for", target_port_alias)

            local target_port = MyObjectManager:lookup(
                    Interest({
                        type = "port",
                        Constraint({ "port.direction", "=", "in" }),
                        Constraint({ "port.alias", "=", target_port_alias }),
                    }))
            if target_port ~= nil then
                print("... creating new link")
                local link = Link("link-factory", {
                    ["link.output.port"] = source_port.properties["object.id"],
                    ["link.input.port"] = target_port.properties["object.id"],
                    ["object.linger"] = true
                })
                link:activate(1)
            end
        end
    end
end

-- We need to disconnect the respective group links, and the create the new ones
-- when the metadata for channel to group mapping changes
SimpleEventHook({
    name = "watch_channel_group_mapping_metadata",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "metadata-changed" }),
            Constraint({ "metadata.name", "=", "performance-mixer" }),
            Constraint({ "event.subject.key", "matches", "channel_group_*" }),
        }),
    },
    execute = function(event)
        print("Channel group metadata changed")
        local properties = event:get_properties()
        local key = properties["event.subject.key"]
        local value = tonumber(properties["event.subject.value"])
        print("Processing key", key, "value", value)

        local group_a_input_id = tonumber(string.sub(key, 15))
        local group_b_input_id = 32 + group_a_input_id

        update_group_assignment(group_a_input_id, value, true)
        update_group_assignment(group_b_input_id, value, false)
    end
}):register()