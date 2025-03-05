SimpleEventHook({
    name = "huh",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "node-added" }),
            Constraint({ "node.name", "=", "pmx-filter-chain-ctrl" }),
        })
    },
    execute = function(event)
        local node = event:get_subject()
        local id = node.properties["object.id"]
        print(id)
        local props = {
            "Spa:Pod:Object:Param:Props",
            "Props",
            params = Pod.Struct { "inputChannels", tonumber(id) },
        }

        local params = Pod.Object(props)

        node:set_param("Props", params)
    end
}):register()