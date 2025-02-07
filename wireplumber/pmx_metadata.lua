Script.async_activation = true

local metadata_name = "performance-mixer"

impl_metadata = ImplMetadata (metadata_name)
impl_metadata:activate (Features.ALL, function (m, e)
    if e then
        Script:finish_activation_with_error (
                "failed to activate the ".. metadata_name .." metadata: " .. tostring (e))
    else
        Script:finish_activation ()
    end
end)
