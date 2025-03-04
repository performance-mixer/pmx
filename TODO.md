# TODO

- Change link creation to only ever use the object.id of the port.
- Change filter setup to use lua scripts again
- Handle removal of nodes and proxies and links etc.
- Use systemd nuit file to set the enabled metadata for pmx

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
