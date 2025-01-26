group_ports_om_A = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"object.path",
			"matches",
			"pmx group mixer A:input_*",
		}),
	}),
})

group_ports_om_B = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"object.path",
			"matches",
			"pmx group mixer B:input_*",
		}),
	}),
})

input_ports_om = ObjectManager({
	Interest({
		type = "port",
		Constraint({
			"object.path",
			"matches",
			"pmx input channels:output_*",
		}),
	}),
})

print("activate oms")

group_ports_om_A:activate()
group_ports_om_B:activate()
input_ports_om:activate()

GroupPortsA = {}
InputPorts = {}

GroupPortsB = {}

print("create tables")
InputPortToIdMapping = {
	[1] = 1,
	[2] = 2,
	[3] = 1,
	[4] = 2,
	[5] = 1,
	[6] = 2,
	[7] = 1,
	[8] = 2,
	[9] = 3,
	[10] = 4,
	[11] = 3,
	[12] = 4,
	[13] = 3,
	[14] = 4,
	[15] = 3,
	[16] = 4,
	[17] = 5,
	[18] = 6,
	[19] = 5,
	[20] = 6,
	[21] = 5,
	[22] = 6,
	[23] = 5,
	[24] = 6,
	[25] = 7,
	[26] = 8,
	[27] = 7,
	[28] = 8,
	[29] = 7,
	[30] = 8,
	[31] = 7,
	[32] = 8,
}

Links = {}

local function connect_A()
	for i = 1, 32 do
		local input_port = InputPorts[i]
		local group_port_id = InputPortToIdMapping[i]
		local group_port = GroupPortsA[group_port_id]
		local link = Link("link-factory", {
			["link.output.port"] = input_port.properties["object.id"],
			["link.input.port"] = group_port.properties["object.id"],
			["link.output.node"] = input_port.properties["node.id"],
			["link.input.node"] = group_port.properties["node.id"],
		})
		link:activate(1)
		table.insert(Links, link)
	end
end

local function connect_B()
	for i = 33, 64 do
		local input_port = InputPorts[i]
		local group_port_id = InputPortToIdMapping[i - 32]
		local group_port = GroupPortsB[group_port_id]
		local link = Link("link-factory", {
			["link.output.port"] = input_port.properties["object.id"],
			["link.input.port"] = group_port.properties["object.id"],
			["link.output.node"] = input_port.properties["node.id"],
			["link.input.node"] = group_port.properties["node.id"],
		})
		link:activate(1)
		table.insert(Links, link)
	end
end

group_ports_om_A:connect("installed", function(om)
	for port in om:iterate() do
		GroupPortsA[port.properties["port.id"] + 1] = port
	end

	if #GroupPortsA == 8 and #GroupPortsB == 8 and #InputPorts == 64 then
		connect_A()
		connect_B()
	end
end)

input_ports_om:connect("installed", function(om)
	for port in om:iterate() do
		InputPorts[port.properties["port.id"] + 1] = port
	end

	if #GroupPortsA == 8 and #GroupPortsB == 8 and #InputPorts == 64 then
		connect_A()
		connect_B()
	end
end)

group_ports_om_B:connect("installed", function(om)
	for port in om:iterate() do
		GroupPortsB[port.properties["port.id"] + 1] = port
	end

	if #GroupPortsA == 8 and #GroupPortsB == 8 and #InputPorts == 64 then
		connect_A()
		connect_B()
	end
end)
