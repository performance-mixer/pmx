SimpleEventHook({
    name = "huh",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
        })
    },
    execute = function(event)
        print(":")
        local props = {
            "Spa:Pod:Object:Param:Props",
            "Props",
            params = Pod.Struct { "inputChannels", 5 },
        }

        local params = Pod.Object(props)

        local node = event:get_subject()
        node:set_param("Props", params)
    end
}):register()