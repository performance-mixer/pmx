local filter_builder = require("pmx_filter_builder")

function build_filter_chains()
    local layer_mixer = filter_builder.build_layer_mixer(LocalModule)
    local group_A = filter_builder.create_group_channels_filter_chain("A",
            LocalModule)
    local group_B = filter_builder.create_group_channels_filter_chain("B",
            LocalModule)
    local input_A = filter_builder.build_input_channels(LocalModule)
    return layer_mixer, group_A, group_B, input_A
end

local layer_mixer, group_A, group_B, input_A = {}, {}, {}, {}

SimpleEventHook({
    name = "pmx/filter_manager/mixer/activation",
    interests = {
        EventInterest({
            Constraint({ "event.type", "=", "metadata-changed" }),
            Constraint({ "metadata.name", "=", "performance-mixer" }),
            Constraint({ "event.subject.key", "=", "enabled" }),
        }),
    },
    execute = function(event)
        local props = event:get_properties()
        local target_key = props["event.subject.key"]
        local target_value = props["event.subject.value"]
        if target_key == "enabled" and target_value == "true" then
            layer_mixer, group_A, group_B, input_A = build_filter_chains()
            print("PMX enabled -> enabling filter")
        else
            layer_mixer, group_A, group_B, input_A = {}, {}, {}, {}
            print("PMX disabled -> disabling filter")
        end
    end,
}):register()
