SimpleEventHook({
    name = "pmx/port_watcher/capture_ports",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "port-added" }),
            Constraint({ "port.direction", "=", "out" }),
            Constraint({ "port.monitor", "!", "true" }),
        }),
    },
    execute = function(event)
        print("hey")
    end,
}):register()

SimpleEventHook({
    name = "test_setup",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
        })
    },
    execute = function(event)
        print("found node")
    end
}):register()

print("shit")
local success, result = pcall(function()

    local props = {
        "Spa:Pod:Object:Param:Props",
        "Props",
        params = Pod.Struct { "inputChannels", 5 },
    }

    return Pod.Object(props)
end)

if not success then
    print("Error occurred:", result)
else
    param = result
end
