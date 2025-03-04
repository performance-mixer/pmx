# TODO

- Change link creation to only ever use the object.id of the port.
- Handle removal of nodes and proxies and links etc.
- Use systemd unit file to set the enabled metadata for pmx, and merge start and
  pmx enable commands in console app

```unit file (systemd)
[Unit]
Description=Example Service

[Service]
Type=simple
ExecStart=/path/to/start_script.sh
ExecStop=/path/to/stop_script.sh
RemainAfterExit=true

[Install]
WantedBy=multi-user.target
```

- wrap commands in console application with function to print error
- setting parameters for filter-chain-ctrl with lua script doesn't work
  correctly, I expect that this has something to do with the types lua assumes
  by default
- SimpleEventHook doesn't work if nodes already exist, need to handle that case
  explicitly 
- 
