local requested_port_alias = "RME RayDAT:capture_AUX0"

om = ObjectManager({
	Interest({ type = "metadata", Constraint({ "metadata.name", "=", "default" }) }),
})

om:activate()

om:connect("installed", function(om)
	for metadata in om:iterate({ type = "metadata" }) do
		for index, key, type, value in metadata:iterate(0) do
			print("Metadata Key:", key, "Type:", type, "Value:", value)
		end
	end
end)
