# TODO

- Change link creation to only ever use the object.id of the port.
- Handle removal of nodes and proxies and links etc.
- Use systemd unit file to set the enabled metadata for pmx, and merge start and
  pmx enable commands in console app
- wrap commands in console application with function to print error

```
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
