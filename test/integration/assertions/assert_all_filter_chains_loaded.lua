local filter_names = {
    "pmx-layer-channels-ins",
    "pmx-layer-channels-outs",
    "pmx-group-channels-a-ins",
    "pmx-group-channels-b-ins",
    "pmx-group-channels-a-outs",
    "pmx-group-channels-b-outs",
    "pmx-input-channels-ins",
    "pmx-input-channels-outs",
}

local node_om = ObjectManager({
    Interest({
        type = "node",
    })
})

node_om:activate()

node_om:connect("installed", function(om)
    for node in om:iterate() do
        for index, name in ipairs(filter_names) do
            if name == node.properties["node.name"] then
                table.remove(filter_names, index)
                break
            end
        end
    end

    if #filter_names == 0 then
        print("Success all filter chains were created")
        return 0
    else
        print("Error not all filter chains were created")
        print("Remaining filter chains: " .. table.concat(filter_names, ", "))
        return -1
    end
end)
