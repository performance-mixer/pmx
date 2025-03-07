# TODO

- Change link creation to only ever use the object.id of the port.
- Handle removal of nodes and proxies and links etc.
- wrap commands in console application with function to print error
- setting parameters for filter-chain-ctrl with lua script doesn't work
  correctly, I expect that this has something to do with the types lua assumes
  by default
- SimpleEventHook doesn't work if nodes already exist, need to handle that case
  explicitly (low prio, might not be strictly necessary, pipewire + wireplumber
  should always start first + load scripts)
