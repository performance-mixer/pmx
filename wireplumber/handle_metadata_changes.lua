SimpleEventHook({
	name = "pmx/process_metadata",
	interests = {
		EventInterest({
			Constraint({ "event.type", "=", "metadata-changed" }),
			Constraint({ "metadata.name", "=", "default" }),
			Constraint({ "event.subject.key", "c", "pmx.group_01" }),
		}),
	},
	execute = function(event)
		print("Metadata changed")
		local source = event:get_source()
		source:call("push-event", "pmx_create_filters", nil, nil)
	end,
}):register()

SimpleEventHook({
	name = "pmx/create_filters",
	interests = {
		EventInterest({
			Constraint({ "event.type", "=", "pmx_create_filters" }),
		}),
	},
	execute = function(event)
		print("Creating filters")
	end,
}):register()
