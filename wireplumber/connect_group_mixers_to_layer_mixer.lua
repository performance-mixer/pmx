layer_ports_om = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"object.path",
			"matches",
			"pmx layer mixer:input_*",
		}),
	}),
})

group_port_om_A = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"object.path",
			"matches",
			"pmx group mixer A:output_*",
		}),
	}),
})

group_port_om_B = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"object.path",
			"matches",
			"pmx group mixer B:output_*",
		}),
	}),
})

layer_ports_om:activate()
group_port_om_A:activate()
group_port_om_B:activate()

GroupPorts = {}
LayerPorts = {}

Links = {}

local function connect()
	print("Connecting")

	for i = 1, 4 do
		local link = Link("link-factory", {
			["link.output.port"] = GroupPorts[i].properties["object.id"],
			["link.input.port"] = LayerPorts[i].properties["object.id"],
			["link.output.node"] = GroupPorts[i].properties["node.id"],
			["link.input.node"] = LayerPorts[i].properties["node.id"],
		})
		link:activate(1)
		Links[i] = link
	end

	print("Connected")
end

layer_ports_om:connect("installed", function(om)
	for port in om:iterate() do
		if port.properties["object.path"] == "pmx layer mixer:input_0" then
			LayerPorts[1] = port
			print(port.properties["object.path"])
		end
		if port.properties["object.path"] == "pmx layer mixer:input_1" then
			LayerPorts[2] = port
			print(port.properties["object.path"])
		end
		if port.properties["object.path"] == "pmx layer mixer:input_2" then
			LayerPorts[3] = port
			print(port.properties["object.path"])
		end
		if port.properties["object.path"] == "pmx layer mixer:input_3" then
			LayerPorts[4] = port
			print(port.properties["object.path"])
		end
	end

	if #GroupPorts == 4 and #LayerPorts == 4 then
		connect()
	end
end)

group_port_om_A:connect("installed", function(om)
	for port in om:iterate() do
		if port.properties["object.path"] == "pmx group mixer A:output_0" then
			GroupPorts[1] = port
			print(port.properties["object.path"])
		end
		if port.properties["object.path"] == "pmx group mixer A:output_1" then
			GroupPorts[2] = port
			print(port.properties["object.path"])
		end
	end

	if #GroupPorts == 4 and #LayerPorts == 4 then
		connect()
	end
end)

group_port_om_B:connect("installed", function(om)
	for port in om:iterate() do
		if port.properties["object.path"] == "pmx group mixer B:output_0" then
			GroupPorts[3] = port
			print(port.properties["object.path"])
		end
		if port.properties["object.path"] == "pmx group mixer B:output_1" then
			GroupPorts[4] = port
			print(port.properties["object.path"])
		end
	end

	if #GroupPorts == 4 and #LayerPorts == 4 then
		connect()
	end
end)
