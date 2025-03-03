local success, result = pcall(function()
    local props = {
        "Spa:Pod:Object:Param:Props",
        "Props",
        params = Pod.Struct { "Compressor:ratio", 4.5 },
    }

    return Pod.Object(props)
end)

if not success then
    print("Error occurred:", result)
else
    param = result
end
