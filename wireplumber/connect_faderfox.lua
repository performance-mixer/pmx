source_port_om = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"port.alias",
			"=",
			"Faderfox PC4:Faderfox PC4 MIDI 1",
		}),
		Constraint({
			"port.direction",
			"=",
			"out",
		}),
	}),
})

target_port_om = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"port.alias",
			"=",
			"pmx-midi-router:input channels",
		}),
		Constraint({
			"port.direction",
			"=",
			"in",
		}),
	}),
})

source_port_om:activate()
target_port_om:activate()

MyLink = nil
local function connect()
	if MyLink == nil then
		for source_port in source_port_om:iterate() do
			for target_port in target_port_om:iterate() do
				MyLink = Link("link-factory", {
					["link.output.port"] = source_port.properties["port.id"],
					["link.input.port"] = target_port.properties["port.id"],
					["link.output.node"] = source_port.properties["node.id"],
					["link.input.node"] = target_port.properties["node.id"],
				})
				MyLink:activate(1)
			end
		end
	end
end

source_port_om:connect("installed", function(om)
	connect()
end)

target_port_om:connect("installed", function(om)
	connect()
end)

source_port_om:connect("object-added", function(om)
	connect()
end)

target_port_om:connect("object-added", function(om)
	connect()
end)
