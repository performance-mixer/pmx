local props = {
    "Spa:Pod:Object:Param:Props",
    "Props",
    ["Compressor-B:attack"] = 1.5,
}

local success, result = pcall(function()
    return Pod.Object(props)
end)

if not success then
    print("Error occurred:", result)
else
    param = result
end
