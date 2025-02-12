local log = Log.open_topic("s-link-manager")
local links = {}

SimpleEventHook({
    name = "pmx/link_manager/create_link",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "pmx_connect_ports" }),
        })
    },
    execute = function(event)
        log:info("connecting ports")
        local properties = event:get_properties()
        local source_port_id = properties["source_port_id"]
        local source_node_id = properties["source_node_id"]
        local target_port_id = properties["target_port_id"]
        local target_node_id = properties["target_node_id"]
        log:info(source_node_id .. source_port_id .. "->" .. target_node_id ..
                target_port_id)
        local link = Link("link-factory", {
            ["link.output.port"] = source_port_id,
            ["link.input.port"] = target_port_id,
            ["link.output.node"] = source_node_id,
            ["link.input.node"] = target_node_id,
            ["linger"] = "true"
        })
        link:activate(1)
        properties["link"] = link
        table.insert(links, properties)
    end,
}):register()
